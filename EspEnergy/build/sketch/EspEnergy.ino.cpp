#include <Arduino.h>
#line 1 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFi.h>

#include "captive_portal.h"
#include "spiffs.h"
#include "measurement.h"
#include "captureTime.h"
#include "utils.h"
#include "rgbLed.h"
#include "time.h"

#define VOLT_PIN 35
#define AMPERE_ONE_PIN 32
#define AMPERE_TWO_PIN 33
#define AMPERE_THREE_PIN 25
#define BUTTON_RESET_PIN 18

//#define DEBUG

TimerHandle_t timer;
QueueHandle_t queue;
TaskHandle_t taskConsumer;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
TaskHandle_t consumer_light;

const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

RTC_DS3231 rtc;

static InternetConfig* conf = new InternetConfig();

char *clientID = "Building0Test";
WiFiClient espClient;
PubSubClient client(espClient);

bool sd = false;
bool wifi = false;

boolean state = false;

#line 47 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void setup();
#line 111 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void loop();
#line 129 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void manageLed();
#line 134 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void ledTask(void * parameter);
#line 158 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void readTask(TimerHandle_t xTimer);
#line 179 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void valueConsumer(void *pvParameters);
#line 192 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void sendMqttData(Measurement measurement);
#line 43 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
void IRAM_ATTR buttonPressed(){
  state = true;
}

void setup()
{
  Serial.begin(115200);
  manageLed();
  pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
  pinMode(VOLT_PIN, INPUT);
  pinMode(AMPERE_ONE_PIN, INPUT);
  pinMode(AMPERE_TWO_PIN, INPUT);
  pinMode(AMPERE_THREE_PIN, INPUT);
  attachInterrupt(BUTTON_RESET_PIN, buttonPressed, FALLING);
  Serial.println("Booting...");
  
  if (initializeSPIFFS()) {
    sd = true;
    InternetConfig* sdConf = readFromFile();
    if (sdConf != NULL) {
      Serial.println("Connecting from sd...");
      Serial.print("Username: ");
      Serial.println(sdConf->username);
      Serial.print("Password: ");
      Serial.println(sdConf->password);
      Serial.print("SSID: ");
      Serial.println(sdConf->ssid);
      selectEncryptionType((wifi_auth_mode_t) 3, sdConf->ssid, sdConf->username, sdConf->password);
      wifi = true;
    } else {
      configureDevice(conf);
      writeToFile("/test.txt", conf);
    }
  } else {
    configureDevice(conf);
  }

  rtc.begin();

  #if !defined(DEBUG)
  client.setServer(conf->broker, 1883);
  client.connect(clientID);
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  Serial.println("Connected to MQTT broker");

  queue = xQueueCreate(10, sizeof(Measurement));
  if (queue == NULL) {
    Serial.println("Couldn't create Queue");
    ESP.restart();
  }
  timer = xTimerCreate("Timer", 5000, pdTRUE, (void *)0, readTask);
  if (timer == NULL) {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }
  
  xTimerStartFromISR(timer, (BaseType_t*)0);

  xTaskCreate(valueConsumer, "consumer", 4096, ( void * ) 1, tskIDLE_PRIORITY, &taskConsumer);
  if (taskConsumer == NULL) {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }
  #endif
}

void loop()
{
  #if defined(DEBUG)
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    if (incomingByte == 'a') {
      DateTime now = rtc.now();
      Measurement m = {0, 1, 2, 3, now};
      writeMeasurementToFile(&m);
    } else if (incomingByte == 'c') {
      readMeasurementFromFile();
    } else if (incomingByte == 'r') {
      resetESP();
    }
  }
  #endif
}

void manageLed(){
  setupLed();
  xTaskCreate(ledTask, "consumer_light", 2048, NULL, 3, NULL);
}

void ledTask(void * parameter){
  byte busStatus = Wire.endTransmission();
  // Repeat every second
  for(;;){
    if(WiFi.status() == WL_CONNECTED){
      setGreenLight();
    }
    if(WiFi.status() == WL_CONNECTED && queue != NULL){
      turnOffLed();
      delay(200);
      setBlueLight();
    }
    if(WiFi.status() == WL_CONNECTED && busStatus==0){
      setRedLight();
      delay(200);
      setBlueLight();
    }
    if(WiFi.status() == WL_DISCONNECTED){
      setRedLight();
    }
    delay(1000);
  }
}

void readTask(TimerHandle_t xTimer) {
  Serial.println(state);
  int buttonState = digitalRead(BUTTON_RESET_PIN);
  Serial.println(buttonState);
  if(state){
    resetESP();
  }
  int volt = analogRead(VOLT_PIN);
  int ampere_one = analogRead(AMPERE_ONE_PIN);
  int ampere_two = analogRead(AMPERE_TWO_PIN);
  int ampere_three = analogRead(AMPERE_THREE_PIN);
  DateTime now = rtc.now();
  printDateTime(now);
  Measurement misura = {volt, ampere_one, ampere_two, ampere_three, now};
  if (xQueueSendFromISR(queue, &misura, &xHigherPriorityTaskWoken )) {
    Serial.println("Object sent");
  } else {
    Serial.println("Queue is full");
  }
}

void valueConsumer(void *pvParameters)
{
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  Measurement received;
  for (;;)
  {
    if (xQueueReceive(queue, &received, ( TickType_t ) 100) == pdPASS)
    {
      sendMqttData(received);
    }
  }
}

void sendMqttData(Measurement measurement) {
  const char* topic = conf->topic;
  if (client.connected() || client.connect(clientID)) {
    Serial.print("Topic is: ");
    Serial.print(conf->topic);
    if (client.publish(topic, toJson(measurement).c_str())) {
      Serial.println(" Publish ok");
      setGreenLight();
    }
    else {
      Serial.println(" Publish failed");
    }
  } else{
  }
}

