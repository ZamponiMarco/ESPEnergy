#line 1 "c:\\Users\\marcz\\OneDrive\\Desktop\\Progetti\\ESPEnergy\\EspEnergy\\microSdCard.h"
#ifndef microSdCard_h
#define microSdCard_h

#include <mySD.h>

typedef struct {
  char ssid[50];
  char username[50];
  char password[50];
} Sdconfig;

void initializeSd();
void writeToSd(String path, String toWritePassword, String toWriteUsername, String ssid);
void printDirectory(File dir, int numTabs);
Sdconfig* readFromSd();


#endif
