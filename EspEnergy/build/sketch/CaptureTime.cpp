#include "captureTime.h"

void printDateTime(DateTime dt){
   char dateBuffer[] = "   DD/MM/YYYY   ";
   char timeBuffer[] = "    hh:mm:ss    ";
   Serial.print(dt.toString(dateBuffer));
   Serial.print(dt.toString(timeBuffer));
}

void saveDateTime(DateTime dt){
}

void sendDateTime(DateTime dt){
}
