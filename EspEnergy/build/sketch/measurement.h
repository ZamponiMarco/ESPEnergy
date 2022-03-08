#ifndef measurement_h
#define measurement_h

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>

typedef struct {
  int volt;
  int ampere_one;
  int ampere_two;
  int ampere_three;
  DateTime timestamp;
} Measurement;

#endif
