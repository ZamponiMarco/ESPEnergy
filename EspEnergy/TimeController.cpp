#include "TimeController.h"

RTC_DS3231 rtc;
struct tm timeinfo;

// SCL 22 SCK 21
void printDateTime(DateTime dt){
   char dateBuffer[] = "   DD/MM/YYYY   ";
   char timeBuffer[] = "    hh:mm:ss    ";
   Serial.print(dt.toString(dateBuffer));
   Serial.print(dt.toString(timeBuffer));
}

void syncRTCtoNTP() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  getLocalTime(&timeinfo);
  rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
}

// Time Sync Task
void syncTime(TimerHandle_t xTimer)
{
  syncRTCtoNTP();
}
