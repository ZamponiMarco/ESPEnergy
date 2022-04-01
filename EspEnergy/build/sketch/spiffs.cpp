#include <SPIFFS.h>

#include "spiffs.h"
#include "measurement.h"

File root;
byte *m = NULL;
uint8_t* inet_config_buf = NULL;

bool initializeSPIFFS()
{
  Serial.println(F("Inizializing FS..."));
  if (SPIFFS.begin())
  {
    Serial.println(F("done."));
  }
  else
  {
    Serial.println(F("fail."));
  }

  // To format all space in SPIFFS
  // SPIFFS.format()

  // Get all information of your SPIFFS

  unsigned int totalBytes = SPIFFS.totalBytes();
  unsigned int usedBytes = SPIFFS.usedBytes();

  Serial.println("File sistem info.");

  Serial.print("Total space:      ");
  Serial.print(totalBytes);
  Serial.println("byte");

  Serial.print("Total space used: ");
  Serial.print(usedBytes);
  Serial.println("byte");

  Serial.println();
  return true;
}

void writeMeasurementToFile(Measurement* m)
{
  root = SPIFFS.open("/measurement.txt", FILE_APPEND);
  if (root)
  {
    root.write((uint8_t *)m, sizeof(Measurement));
    root.flush();
    root.close();
  }
}

void writeToFile(String path, InternetConfig* conf)
{
  root = SPIFFS.open(path.c_str(), FILE_WRITE);
  if (root)
  {
    root.write((uint8_t *)conf, sizeof(InternetConfig));
    root.flush();
    root.close();
  }
  else
  {
    Serial.println("error opening file.txt");
  }
}

/*void printDirectory(File dir, int numTabs) {
  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');   // we'll have a nice indentation
     }
     // Print the name
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       Serial.print("\t\t");
       Serial.println(entry.size());
     }
     entry.close();
   }
}*/

InternetConfig *readFromFile()
{
  if (!SPIFFS.exists("/test.txt")) {
    Serial.println("Config doesn't exist");
    return NULL;
  }
  root = SPIFFS.open("/test.txt");
  if (root)
  {
    inet_config_buf = (byte *)malloc(sizeof(InternetConfig));
    root.read(inet_config_buf, sizeof(InternetConfig));
    root.close();
    return (InternetConfig *)inet_config_buf;
  }
  else
  {
    Serial.println("error opening file .txt");
    return NULL;
  }
}

Measurement *readMeasurementFromFile()
{
  root = SPIFFS.open("/measurement.txt");
  if (root)
  {
    while (root.available())
    {
      m = (byte *)malloc(sizeof(Measurement));
      root.read(m, sizeof(Measurement));
      Serial.print("Ampere: ");
      Serial.println(((Measurement *)m)->ampere_one);
    }
    root.close();
    SPIFFS.remove("/measurement.txt");
    return (Measurement *)m;
  }
  else
  {
    Serial.println("error opening file .txt");
    return NULL;
  }
}

void resetESP(){
  if (SPIFFS.exists("/test.txt"))
  {
    SPIFFS.remove("/test.txt");
  }
  if (SPIFFS.exists("/measurement.txt"))
  {
    SPIFFS.remove("/measurement.txt");
  }
  ESP.restart();
}
