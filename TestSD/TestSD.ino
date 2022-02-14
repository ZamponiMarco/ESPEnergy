#include "measurement.h"
#include <mySD.h>

File root;
Measurement misura1 = {3, 0, 1, 2, 0};
Measurement misura2 = {3, 0, 1, 2, 1};
Measurement misura3 = {3, 0, 1, 2, 2};
Measurement *m = NULL;

void initializeSd(){
  Serial.print("Initializing SD card...");
  /* initialize SD library with Soft SPI pins, if using Hard SPI replace with this SD.begin()*/
  if (!SD.begin(26, 14, 13, 27)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void readFromSd(){
  root = SD.open("test.txt");
  if (root) {
    while(root.available()){
      if(m != NULL){
        free(m);
      }
      m = (Measurement*) malloc(sizeof(Measurement));
      root.read(m, sizeof(Measurement));
      Serial.println(m->timestamp);
    }
    root.close();
    SD.remove("test.txt");
  } else {
    Serial.println("error opening file .txt");
  }
}

void write(){
  root = SD.open("test.txt", FILE_WRITE);
  Serial.print("done1");
  if (root) {
    Serial.print("done2");
    root.write((uint8_t*) &misura1,sizeof(Measurement));
    root.flush();
    root.write((uint8_t*) &misura2,sizeof(Measurement));
    root.flush();
    root.write((uint8_t*) &misura3,sizeof(Measurement));
    root.flush();
    Serial.println("done3");
    root.close();
  }
} 

void setup(){
  Serial.begin(115200);
  initializeSd();
  write();
  write();
  readFromSd();
}

void loop(){
  
}
