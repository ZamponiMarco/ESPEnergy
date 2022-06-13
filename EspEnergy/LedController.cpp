#include "LedController.h"

void setupLed(){
    pinMode(RED_LIGHT_PIN, OUTPUT);
    pinMode(GREEN_LIGHT_PIN, OUTPUT);
    pinMode(BLUE_LIGHT_PIN, OUTPUT);
}

void setRedLight(){
  digitalWrite(RED_LIGHT_PIN, HIGH);
  digitalWrite(GREEN_LIGHT_PIN, LOW);
  digitalWrite(BLUE_LIGHT_PIN, LOW);
}

void setGreenLight(){
  digitalWrite(RED_LIGHT_PIN, LOW);
  digitalWrite(GREEN_LIGHT_PIN, HIGH);
  digitalWrite(BLUE_LIGHT_PIN, LOW);
}

void setBlueLight(){
  digitalWrite(RED_LIGHT_PIN, LOW);
  digitalWrite(GREEN_LIGHT_PIN, LOW);
  digitalWrite(BLUE_LIGHT_PIN, HIGH);
}

void turnOffLed(){
  digitalWrite(RED_LIGHT_PIN, LOW);
  digitalWrite(GREEN_LIGHT_PIN, LOW);
  digitalWrite(BLUE_LIGHT_PIN, LOW);
}

void setRgbColor(int red_light_value, int green_light_value, int blue_light_value){
    digitalWrite(RED_LIGHT_PIN, red_light_value);
    digitalWrite(GREEN_LIGHT_PIN, green_light_value);
    digitalWrite(BLUE_LIGHT_PIN, blue_light_value);
} 

void ledTask(void * parameter){
  setupLed();
  for(;;){
    if(WiFi.status() == WL_CONNECTED){
      setGreenLight();
    } else {
      setRedLight();
    }
    vTaskDelay(1000);
  }
}
