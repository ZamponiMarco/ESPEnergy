#pragma once 

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <TimeLib.h>

#define GMT_OFFSET_SEC 3600
#define DAYLIGHT_OFFSET_SEC 3600
#define NTP_SERVER "pool.ntp.org"

extern RTC_DS3231 rtc;
extern struct tm timeinfo;

void printDateTime(DateTime dt);
void syncRTCtoNTP();
void syncTime(TimerHandle_t xTimer);
