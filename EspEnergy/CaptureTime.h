#ifndef capturetime_h
#define capturetime_h

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include "SD.h"

typedef struct{
  String dateBuffer;
  String timeBuffer;
} RtcDateTime;

void printDateTime(DateTime dt, RtcDateTime rtcDateTime);
void saveDateTime(DateTime dt, RtcDateTime rtcDateTime);
void sendDateTime(DateTime dt, RtcDateTime rtcDateTime);

#endif
