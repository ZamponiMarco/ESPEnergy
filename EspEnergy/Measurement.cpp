#include "Measurement.h"

WiFiClient espClient;
PubSubClient client(espClient);

QueueHandle_t queue;

void readTask(TimerHandle_t xTimer)
{
  int volt = analogRead(VOLT_PIN);
  int ampere_one = analogRead(AMPERE_ONE_PIN);
  int ampere_two = analogRead(AMPERE_TWO_PIN);
  int ampere_three = analogRead(AMPERE_THREE_PIN);

  DateTime now = rtc.now();
  printDateTime(now);

  Measurement misura = {volt, ampere_one, ampere_two, ampere_three, now};
  if (xQueueSendFromISR(queue, &misura, NULL))
  {
    Serial.println("Object sent");
  }
  else
  {
    Serial.println("Queue is full");
  }
}

void valueConsumer(void *pvParameters)
{
  configASSERT(((uint32_t)pvParameters) == 1);
  Measurement received;
  for (;;)
  {
    if (xQueueReceive(queue, &received, (TickType_t)100) == pdPASS)
    {
      sendMqttData(received);
    }
  }
}

void sendMqttData(Measurement measurement)
{
  const char *topic = conf->topic;
  if (client.connected() || client.connect(CLIENT_ID))
  {
    Serial.print("Topic is: ");
    Serial.print(conf->topic);
    if (client.publish(topic, toJson(measurement).c_str()))
    {
      Serial.println(" Publish ok");
    }
    else
    {
      Serial.println(" Publish failed");
    }
  }
}

String toJson(Measurement dataVariable) {
  JSONVar toSend;
  toSend["volt"] = dataVariable.volt;
  toSend["ampere_one"] = dataVariable.ampere_one;
  toSend["ampere_two"] = dataVariable.ampere_two;
  toSend["ampere_three"] = dataVariable.ampere_three;
  toSend["time"] =  dataVariable.timestamp.timestamp();
  return JSON.stringify(toSend);
}

double scale(int analog, double min, double max) {
  double step = (max - min) / 4095.0;
  return analog * step + min;
}
