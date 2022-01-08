#include "microSdCard.h"

File root;

void writeToSd(String path, String toWrite)
{
  Serial.print("Initializing SD card...");
  /* initialize SD library with Soft SPI pins, if using Hard SPI replace with this SD.begin()*/
  if (!SD.begin(26, 14, 13, 27)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  root = SD.open(path.c_str(), FILE_WRITE);
  if (root) {
    root.println(toWrite);
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

/*String readFromSd(){
  if (!SD.begin(26, 14, 13, 27)) {
    Serial.println("initialization failed!");
  }
  root = SD.open("test.txt");
  if (root) {    
    while (root.available()) {
      Serial.write(root.read());
    }
    root.close();
    return root.read();
  } else {
    Serial.println("error opening file .txt");
  }
}*/
