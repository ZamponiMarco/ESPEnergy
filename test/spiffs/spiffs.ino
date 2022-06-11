#include "SPIFFS.h"
#include "measurement.h"

File root;
Measurement misura1 = {3, 0, 1, 2, 0};
Measurement misura2 = {3, 0, 1, 2, 1};
Measurement misura3 = {3, 0, 1, 2, 2};
byte *m = NULL;
 
void setup()
{
    Serial.begin(115200);
    delay(500);
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
    write();
    // Cycle all the content
    root.close();
}
 
void loop()
{
  Serial.print("Leggo nel file txt: \n");
  root = SPIFFS.open("/test.txt");
  if (root) {
    while(root.available()){
      if(m != NULL){
        free(m);
      }
      m = (byte*) malloc(sizeof(Measurement));
      root.read(m, sizeof(Measurement));
      //root.read();
      Serial.println(((Measurement *)m)->timestamp);
    }
    root.close();
    //SPIFFS.remove("/test.txt");
  } else {
    Serial.println("error opening file .txt");
  }
  delay(1000);
}

void write(){
    // Open root folder
    root = SPIFFS.open("/test.txt", FILE_WRITE);
    Serial.print("done1");
    root.write((uint8_t*) &misura1,sizeof(Measurement));
    root.flush();
    root.write((uint8_t*) &misura2,sizeof(Measurement));
    root.flush();
    root.write((uint8_t*) &misura3,sizeof(Measurement));
    root.flush();
    Serial.print("Ho scritto i measurement");
}
 
void printDirectory() {
}
