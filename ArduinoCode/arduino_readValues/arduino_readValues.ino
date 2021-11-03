#include <Arduino_FreeRTOS.h>
#include <timers.h>
#include <task.h>

double volt;
double ampere;
TimerHandle_t timer;

void setup() {
  Serial.begin(9600);
  pinMode(A5,INPUT);
  pinMode(A4,INPUT);

  timer = xTimerCreate("Timer", pdMS_TO_TICKS( 1000 ), pdTRUE, ( void * ) 0, valueRead);
  xTimerStartFromISR(timer,0);
  vTaskStartScheduler();
}

void loop(){
   
}

void valueRead( TimerHandle_t xTimer ){
  volt=analogRead(A5);
  ampere=analogRead(A4);
  Serial.print("volt: ");
  Serial.print(volt);
  Serial.print(", ampere: ");
  Serial.print(ampere);
  Serial.println("");
}
