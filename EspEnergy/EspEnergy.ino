#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <Arduino_JSON.h>

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

char *clientID = "Building0Test";
char* topic = "esptest";
char* server = "broker.emqx.io";
WiFiClient espClient;
PubSubClient client(espClient);

bool sd = false;
bool wifi = false;

void setup()
{
  Serial.begin(115200);
  pinMode(VOLT_PIN, INPUT);
  pinMode(AMPERE_ONE_PIN, INPUT);
  pinMode(AMPERE_TWO_PIN, INPUT);
  pinMode(AMPERE_THREE_PIN, INPUT);
  Serial.println("Booting...");
  rtc.begin();
  if (initializeSd()) {
    sd = true;
    Sdconfig* sdConf = readFromSd();
    if (sdConf != NULL) {
      Serial.println("Connecting from sd...");
      Serial.println(sdConf->username);
      Serial.println(sdConf->password);
      Serial.println(sdConf->ssid);
      Measurement* misureSd = readMeasurementFromSd();
      if (misureSd != NULL) {
        Serial.println("Reading values from sd...");
        Serial.println(misureSd->ampere_one);
        Serial.println(misureSd->ampere_two);
        Serial.println(misureSd->ampere_three);
      }
      selectEncryptionType((wifi_auth_mode_t) 3, sdConf->ssid, sdConf->username, sdConf->password);
      wifi = true;
    } else {
      configureDevice(conf);
      writeToSd("test.txt" , conf->password, conf->username, conf->ssid);
    }
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
      sendMqttData(received);
    } else {
      Serial.println("queue empty");
    }
  }
}

void sendMqttData(Measurement measurement) {
  client.setServer(server, 1883);
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(topic);
    if (client.publish(topic, toJson(measurement).c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
      writeMeasurementSd();
    }
  } else{
    writeMeasurementSd();
  }
}

String toJson(Measurement dataVariable) {
  JSONVar toSend;
  toSend["volt"] = dataVariable.volt;
  toSend["ampere_one"] = dataVariable.ampere_one;
  toSend["ampere_two"] = dataVariable.ampere_two;
  toSend["ampere_three"] = dataVariable.ampere_three;
  toSend["time"] = dataVariable.timestamp;
  return JSON.stringify(toSend);
}

double scale(int analog, double min, double max) {
  double step = (max - min) / 4095.0;
  return analog * step + min;
}