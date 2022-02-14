#line 1 "c:\\Users\\marcz\\OneDrive\\Desktop\\Progetti\\ESPEnergy\\EspEnergy\\CaptureTime.h"
#ifndef capturetime_h
#define capturetime_h

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>

void printDateTime(DateTime dt);
void saveDateTime(DateTime dt);
void sendDateTime(DateTime dt);

#endif
