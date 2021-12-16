#include "CaptivePortal.h"
#include "measurement.h"

#define VOLT_PIN 35
#define AMPERE_ONE_PIN 32
#define AMPERE_TWO_PIN 33
#define AMPERE_THREE_PIN 25

TimerHandle_t timer;
QueueHandle_t queue;
TaskHandle_t taskConsumer;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;

static Configuration* conf = new Configuration();

void setup()
{
  Serial.begin(115200);
  startWebServer(conf);
  Serial.println(conf->ssid);

  /*

    pinMode(VOLT_PIN, INPUT);
    pinMode(AMPERE_ONE_PIN, INPUT);
    pinMode(AMPERE_TWO_PIN, INPUT);
    pinMode(AMPERE_THREE_PIN, INPUT);

    queue = xQueueCreate(10, sizeof(measurement));

    if (queue == NULL) {
    Serial.println("Couldn't create Queue");
    ESP.restart();
    }

    timer = xTimerCreate("Timer", 1000, pdTRUE, (void *)0, readTask);

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
  */
}

void loop()
{
}


/*
void readTask(TimerHandle_t xTimer) {
  int volt = analogRead(VOLT_PIN);
  int ampere_one = analogRead(AMPERE_ONE_PIN);
  int ampere_two = analogRead(AMPERE_TWO_PIN);
  int ampere_three = analogRead(AMPERE_THREE_PIN);
  struct measurement misura = {volt, ampere_one, ampere_two, ampere_three, 0};
  if (xQueueSendFromISR(queue, &misura, &xHigherPriorityTaskWoken )) {
    Serial.println("Object sent");
  } else {
    Serial.println("Queue is full");
  }
}

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
      Serial.print(", ampere1: ");
      Serial.print(received.ampere1);
      Serial.print(", ampere2: ");
      Serial.print(received.ampere2);
      Serial.print(", ampere3: ");
      Serial.print(received.ampere3);
      Serial.print(", timestamp: ");
      Serial.print(received.timestamp);
      Serial.println("}");
    } else {
      Serial.println("queue empty");
    }
  }
}

*/
