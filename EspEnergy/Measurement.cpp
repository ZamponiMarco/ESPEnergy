#include "Measurement.h"

String toJson(Measurement dataVariable) {
  JSONVar toSend;
  toSend["volt"] = dataVariable.volt;
  toSend["ampere_one"] = dataVariable.ampere_one;
  toSend["ampere_two"] = dataVariable.ampere_two;
  toSend["ampere_three"] = dataVariable.ampere_three;
  toSend["time"] =  dataVariable.timestamp.timestamp();
  return JSON.stringify(toSend);
}

double scale(int analog, double min, double max) {
  double step = (max - min) / 4095.0;
  return analog * step + min;
}
