#include "microSdCard.h"

File root;
String fileContent;

void initializeSd(){
  Serial.print("Initializing SD card...");
  /* initialize SD library with Soft SPI pins, if using Hard SPI replace with this SD.begin()*/
  if (!SD.begin(26, 14, 13, 27)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void writeToSd(String path, String toWritePassword, String toWriteUsername)
{
  root = SD.open(path.c_str(), FILE_WRITE);
  if (root) {
    root.println("Username: ");
    root.println(toWriteUsername);
    root.println("Password: ");
    root.println(toWritePassword);
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

String readFromSd(){
  root = SD.open("test.txt");
  String buffer;
  String datas;
  if (root) {    
    while (root.available()) {
      buffer = root.readStringUntil(':');
      datas = root.readStringUntil('\n');
      Serial.print("Datas: " + datas);
      Serial.write(root.read());
    }
    root.close();
    fileContent = root.read();
    return fileContent;
  } else {
    Serial.println("error opening file .txt");
  }
}
