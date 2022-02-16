#ifndef measurement_h
#define measurement_h

typedef struct {
  int volt;
  int ampere_one;
  int ampere_two;
  int ampere_three;
  int timestamp;
} Measurement;

#endif
