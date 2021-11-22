#include "measurement.h"

TimerHandle_t timer;
QueueHandle_t queue;
TaskHandle_t taskConsumer;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;

void setup()
{
  Serial.begin(115200);
  pinMode(A5, INPUT);
  pinMode(A4, INPUT);

  Serial.println("Starting Program");

  queue = xQueueCreate(10, sizeof(measurement));

  if (queue == NULL) {
    Serial.println("Couldn't create Queue");
    ESP.restart();
  }

  timer = xTimerCreate("Timer", 1000, pdTRUE, (void *)0, pippo);

  if (timer == NULL) {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerStartFromISR(timer, (BaseType_t*)0);
  xTaskCreate(valueConsumer, "consumer", 1024, ( void * ) 1, tskIDLE_PRIORITY, &taskConsumer);

  if (taskConsumer == NULL) {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }
}

void loop()
{
}


void pippo(TimerHandle_t xTimer) {
  struct measurement misura;
  if (xQueueSendFromISR(queue, &misura, &xHigherPriorityTaskWoken )) {
    Serial.println("Object sent");
  } else {
    Serial.println("Queue is full");
  }
}

/* Task to be created. */
void valueConsumer(void *pvParameters)
{
  configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
  struct measurement received;
  for (;;)
  {
    if (xQueueReceive(queue, &received, ( TickType_t ) 1000) == pdPASS)
    {
      Serial.print("Object received: {volt: ");
      Serial.print(received.volt);
      Serial.print(", ampere: ");
      Serial.print(received.ampere);
      Serial.println("}");
    } else {
      Serial.println("queue empty");
    }
  }
}
