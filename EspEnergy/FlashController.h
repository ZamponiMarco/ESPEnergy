#pragma once

#include <SPIFFS.h>

#include "Measurement.h"
#include "ConnectionController.h"

bool initializeSPIFFS();
void writeToFile(String path, InternetConfig* conf);
//void printDirectory(File dir, int numTabs);
InternetConfig* readFromFile();
void writeMeasurementToFile(Measurement* m);
Measurement* readMeasurementFromFile();
void resetESP();
