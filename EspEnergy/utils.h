#include <Arduino_JSON.h>

#include "measurement.h"

String toJson(Measurement dataVariable);
double scale(int analog, double min, double max);