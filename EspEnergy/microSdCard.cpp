#include "microSdCard.h"

File root;
String fileContent;
Sdconfig* sdconfig = NULL;

void initializeSd(){
  Serial.print("Initializing SD card...");
  /* initialize SD library with Soft SPI pins, if using Hard SPI replace with this SD.begin()*/
  if (!SD.begin(26, 14, 13, 27)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
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
  String datas;
  if (root) {    
    while (root.available()) {
      if(sdconfig != NULL){
        free(sdconfig);
      }
      sdconfig = (Sdconfig*) malloc(root.size());
      buffer = root.read(sdconfig, root.size());
    }
    root.close();
    return sdconfig;
  } else {
    Serial.println("error opening file .txt");
    return NULL;
  }
}
