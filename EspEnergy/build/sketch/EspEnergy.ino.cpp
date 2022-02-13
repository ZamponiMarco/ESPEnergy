#include <Arduino.h>
#line 1 "c:\\Users\\marcz\\OneDrive\\Desktop\\Progetti\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFi.h>

#include "captive_portal.h"
#include "microSdCard.h"
#include "measurement.h"
#include "captureTime.h"

#define VOLT_PIN 35
#define AMPERE_ONE_PIN 32
#define AMPERE_TWO_PIN 33
#define AMPERE_THREE_PIN 25

TimerHandle_t timer;
QueueHandle_t queue;
TaskHandle_t taskConsumer;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
RTC_DS3231 rtc;

static Configuration* conf = new Configuration();

const char* ssid = "POCO M3";
const char* password = "culocane";

char *clientID = "Building0Test";
char* topic = "test";
char* server = "broker.emqx.io";
WiFiClient espClient;
PubSubClient client(espClient);
String sdContent;

void setup()
{
  Serial.begin(115200);
  pinMode(VOLT_PIN, INPUT);
  pinMode(AMPERE_ONE_PIN, INPUT);
  pinMode(AMPERE_TWO_PIN, INPUT);
  pinMode(AMPERE_THREE_PIN, INPUT);
  rtc.begin();
  initializeSd();
  Serial.println("Contenuto della sd: ");
  Sdconfig* sdConf = readFromSd();
  if (sdConf != NULL) {
    Serial.println("Connecting from sd...");
    Serial.println(sdConf->username);
    Serial.println(sdConf->password);
    Serial.println(sdConf->ssid);
    //Encryption type
    selectEncryptionType((wifi_auth_mode_t) 3, sdConf->ssid, sdConf->username, sdConf->password);
  } else {
    configureDevice(conf);
    writeToSd("test.txt" , conf->password, conf->username, conf->ssid);
  }

  queue = xQueueCreate(10, sizeof(Measurement));
  if (queue == NULL) {
    Serial.println("Couldn't create Queue");
    ESP.restart();
  }
  timer = xTimerCreate("Timer", 1000, pdTRUE, (void *)0, readTask);
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
}

void loop()
{
}

void readTask(TimerHandle_t xTimer) {
  int volt = analogRead(VOLT_PIN);
  int ampere_one = analogRead(AMPERE_ONE_PIN);
  int ampere_two = analogRead(AMPERE_TWO_PIN);
  int ampere_three = analogRead(AMPERE_THREE_PIN);
  DateTime now = rtc.now();
  printDateTime(now);
  Measurement misura = {volt, ampere_one, ampere_two, ampere_three, 0};
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
    if (xQueueReceive(queue, &received, ( TickType_t ) 1000) == pdPASS)
    {
      sendMqttData("pippo");
    } else {
      Serial.println("queue empty");
    }
  }
}

void sendMqttData(String dataVariable) {
  client.setServer(server, 1883);
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(topic);
    if (client.publish(topic, dataVariable.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
}

