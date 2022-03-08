#ifndef spiffs_h
#define spiffs_h
#include "measurement.h"

#include <mySD.h>

typedef struct {
  char ssid[50];
  char username[50];
  char password[50];
} InternetConfig;

bool initializeSPIFFS();
void writeToFile(String path, String toWritePassword, String toWriteUsername, String ssid);
void printDirectory(File dir, int numTabs);
InternetConfig* readFromFile();
void writeMeasurementToFile();
Measurement* readMeasurementFromFile();

#endif
