#include "microSdCard.h"
#include "measurement.h"

File root;
String fileContent;
Sdconfig* sdconfig = NULL;
Measurement* measurement = NULL;

bool initializeSd(){
  Serial.print("Initializing SD card...");
  /* initialize SD library with Soft SPI pins, if using Hard SPI replace with this SD.begin()*/
  if (!SD.begin(26, 14, 13, 27)) {
    Serial.println("initialization failed!");
    return false;
  }
  Serial.println("initialization done.");
  return true;
}

void writeMeasurementSd(){
  root = SD.open("measurement.txt", FILE_WRITE);
  Serial.print("done1");
  if (root) {
    Serial.print("done2");
    root.write((uint8_t*) &measurement,sizeof(Measurement));
    root.flush();
    Serial.println("done3");
    root.close();
  }
} 

void writeToSd(String path, String toWritePassword, String toWriteUsername, String toWriteSsid)
{
  root = SD.open(path.c_str(), FILE_WRITE);
  if (root) {
    Sdconfig config;
    //mi serve: ssid, username, password, tipo di cifratura
    toWriteUsername.toCharArray(config.username, toWriteUsername.length()+1);
    toWritePassword.toCharArray(config.password, toWritePassword.length()+1);
    toWriteSsid.toCharArray(config.ssid, toWriteSsid.length()+1);
    root.write((uint8_t*) &config,sizeof(Sdconfig));
    root.flush();
   /* close the file */
    root.close();
  } else {
    Serial.println("error opening file.txt");
  }
}

void printDirectory(File dir, int numTabs) {
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
     /* Recurse for directories, otherwise print the file size */
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       /* files have sizes, directories do not */
       Serial.print("\t\t");
       Serial.println(entry.size());
     }
     entry.close();
   }
}

Sdconfig* readFromSd(){
  root = SD.open("test.txt");
  int buffer;
  int bufferMeasurement;
  String datas;
  if (root) {    
    while (root.available()) {
      if(sdconfig != NULL){
        free(sdconfig);
      }
      sdconfig = (Sdconfig*) malloc(sizeof(Sdconfig));
      buffer = root.read(sdconfig, sizeof(Sdconfig));
    }
    root.close();
    return sdconfig;
  } else {
    Serial.println("error opening file .txt");
    return NULL;
  }
}

Measurement* readMeasurementFromSd(){
  root = SD.open("measurement.txt");
  if (root) {    
    while (root.available()) {
      if(sdconfig != NULL){
        free(sdconfig);
      }
      measurement = (Measurement*) malloc(sizeof(Measurement));
      root.read(measurement, sizeof(Measurement));
    }
    root.close();
    return measurement;
  } else {
    Serial.println("error opening file .txt");
    return NULL;
  }
}
