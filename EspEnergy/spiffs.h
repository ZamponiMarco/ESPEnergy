#ifndef spiffs_h
#define spiffs_h

#include <SPIFFS.h>

#include "measurement.h"
#include "captive_portal.h"

bool initializeSPIFFS();
void writeToFile(String path, InternetConfig* conf);
//void printDirectory(File dir, int numTabs);
InternetConfig* readFromFile();
void writeMeasurementToFile(Measurement* m);
Measurement* readMeasurementFromFile();
void resetESP();

#endif
