#pragma once

#include <RTClib.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

#include "TimeController.h"
#include "ConnectionController.h"

#define VOLT_PIN 35
#define AMPERE_ONE_PIN 32
#define AMPERE_TWO_PIN 33
#define AMPERE_THREE_PIN 25

#define CLIENT_ID "Building0Test"

extern WiFiClient espClient;
extern PubSubClient client;

extern QueueHandle_t queue;

typedef struct {
  int volt;
  int ampere_one;
  int ampere_two;
  int ampere_three;
  DateTime timestamp;
} Measurement;

void readTask(TimerHandle_t xTimer);
void valueConsumer(void *pvParameters);
void sendMqttData(Measurement measurement);
String toJson(Measurement dataVariable);
double scale(int analog, double min, double max);
