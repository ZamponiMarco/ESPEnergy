#ifndef measurement_h
#define measurement_h

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>

typedef struct {
  int volt;
  int ampere1;
  int ampere2;
  int ampere3;
  int timestamp;
} Measurement;

#endif
