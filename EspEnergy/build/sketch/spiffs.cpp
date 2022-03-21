#include <SPIFFS.h>

#include "spiffs.h"
#include "measurement.h"

File root;
String fileContent;
byte *internetConfig = NULL;
Measurement* measurement = NULL;
byte *m = NULL;

bool initializeSPIFFS(){
Serial.println(F("Inizializing FS..."));
    if (SPIFFS.begin()){
        Serial.println(F("done."));
    }else{
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

void writeMeasurementToFile(){
  root = SPIFFS.open("measurement.txt", FILE_WRITE);
  Serial.print("done1");
  if (root) {
    Serial.print("done2");
    root.write((uint8_t*) &measurement,sizeof(Measurement));
    root.flush();
    Serial.println("done3");
    root.close();
  }
} 

void writeToFile(String path, String toWritePassword, String toWriteUsername, String toWriteSsid)
{
  root = SPIFFS.open(path.c_str(), FILE_WRITE);
  if (root) {
    InternetConfig config;
    //mi serve: ssid, username, password, tipo di cifratura
    toWriteUsername.toCharArray(config.username, toWriteUsername.length()+1);
    toWritePassword.toCharArray(config.password, toWritePassword.length()+1);
    toWriteSsid.toCharArray(config.ssid, toWriteSsid.length()+1);
    root.write((uint8_t*) &config,sizeof(InternetConfig));
    root.flush();
   /* close the file */
    root.close();
  } else {
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

InternetConfig* readFromFile(){
  root = SPIFFS.open("/test.txt");
  int bufferMeasurement;
  String datas;
  if (root) {    
    while (root.available()) {
      if(internetConfig != NULL){
        free(internetConfig);
      }
      internetConfig = (byte*) malloc(sizeof(InternetConfig));
      root.read(internetConfig, sizeof(InternetConfig));
    }
    root.close();
    return (InternetConfig*)internetConfig;
  } else {
    Serial.println("error opening file .txt");
    return NULL;
  }
}

Measurement* readMeasurementFromFile(){
  root = SPIFFS.open("/test.txt");
  if (root) {    
    while (root.available()) {
      if(internetConfig != NULL){
        free(internetConfig);
      }
      m = (byte*) malloc(sizeof(Measurement));
      root.read(m, sizeof(Measurement));
      Serial.println(((Measurement *)m)->ampere_one);
    }
    root.close();
    return (Measurement *)m;
  } else {
    Serial.println("error opening file .txt");
    return NULL;
  }
}
