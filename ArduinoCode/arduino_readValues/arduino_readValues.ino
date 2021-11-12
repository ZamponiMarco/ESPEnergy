#include <Arduino_FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <queue.h>

#include "measurement.h"

int volt;
int ampere;
TimerHandle_t timer;
QueueHandle_t queue;
TaskHandle_t taskConsumer;
TaskHandle_t taskProducer;
void setup()
{
  Serial.begin(9600);
  pinMode(A5, INPUT);
  pinMode(A4, INPUT);
  
  queue = xQueueCreate(100, sizeof(measurement));
  Serial.println("start");
  //xTaskCreate(valueConsumer,"consumer",200, (void *)0, tskIDLE_PRIORITY, &taskConsumer);
  xTaskCreate(valueProducer,"producer",200, (void *)0, 1, &taskProducer);
  vTaskStartScheduler();
}

void loop()
{
}

void valueProducer(void *pvParameters)
{
  timer = xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdFALSE, (void *)0, pippo);
  xTimerStartFromISR(timer, (BaseType_t*)0);
}


void pippo(TimerHandle_t xTimer){
  volt = analogRead(A5);
  ampere = analogRead(A4);
  measurement misura = {volt, ampere, 0};
  xQueueSendFromISR(queue, &misura, (BaseType_t*)0);
  Serial.println("Timer work?");
}

/* Task to be created. */
void valueConsumer(void *pvParameters)
{
  measurement *pointerMeasurement;
  // configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  for (;;)
  {
    Serial.println("for");
    /* Task code goes here. */
    if (xQueueReceive(queue, pointerMeasurement, pdMS_TO_TICKS(100)))
    {
      //Serial.print("volt: ");
      //Serial.print(pointerMeasurement->volt);
      //Serial.print(", ampere: ");
      //Serial.print(pointerMeasurement->ampere);
      //Serial.println("");
    }
  }
}
