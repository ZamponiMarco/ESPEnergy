#pragma once 

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <TimeLib.h>

extern RTC_DS3231 rtc;

void printDateTime(DateTime dt);
void saveDateTime(DateTime dt);
void sendDateTime(DateTime dt);
