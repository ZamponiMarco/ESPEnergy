#include "MeasurementController.h"

WiFiClient espClient;
PubSubClient client(espClient);

QueueHandle_t queue;

// Measurement Producer Task
void readTask(TimerHandle_t xTimer)
{
  // Measurement Production
  int volt = analogRead(VOLT_PIN);
  int ampere_one = analogRead(AMPERE_ONE_PIN);
  int ampere_two = analogRead(AMPERE_TWO_PIN);
  int ampere_three = analogRead(AMPERE_THREE_PIN);

  DateTime now = rtc.now();
  printDateTime(now);

  Measurement misura = {volt, ampere_one, ampere_two, ampere_three, now};


  // Sending to FreeRTOS Queue
  if (xQueueSendFromISR(queue, &misura, NULL))
  {
    Serial.print("Object sent");
  } else
  {
    Serial.println("Queue is full");
  }
}

// Measurement Consumer Task
void valueConsumer(void *pvParameters)
{
  configASSERT(((uint32_t)pvParameters) == 1);

  // Measurement Retrieval
  Measurement received;
  for (;;)
  {
    if (xQueueReceive(queue, &received, (TickType_t)100) == pdPASS)
    {
      // Measurement Usage
      if (!sendMqttData(received)) 
      {
        writeMeasurementToFile(&received);
      }
    }
  }
}

bool sendMqttData(Measurement measurement)
{
  if (!client.connected() && !client.connect(CLIENT_ID)) 
  {
    Serial.println(" Publish failed");
    return false;
  }

  Serial.print("Topic is: ");
  Serial.print(conf->topic);
  
  if (client.publish(conf->topic, toJson(measurement).c_str())) 
  {
    Serial.println(" Publish ok");
    return true;
  } else
  {
    Serial.println(" Publish failed");
    return false;
  }
}
