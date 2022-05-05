#pragma once

#include <Arduino.h>
#include <WiFi.h>

#define RED_LIGHT_PIN 4
#define GREEN_LIGHT_PIN 16
#define BLUE_LIGHT_PIN 17

void setupLed();
void setRedLight();
void setGreenLight();
void setBlueLight();
void turnOffLed();
void setRgbColor(int red_light_value, int green_light_value, int blue_light_value);
void ledTask(void * parameter);
