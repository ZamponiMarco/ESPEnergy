#pragma once

#include <SPIFFS.h>

#include "Measurement.h"
#include "ConnectionController.h"

bool initializeSPIFFS();
void writeToFile(String path, InternetConfig* conf);
InternetConfig* readFromFile();
void writeMeasurementToFile(Measurement* m);
void readMeasurementFromFile();
void resetESP();
