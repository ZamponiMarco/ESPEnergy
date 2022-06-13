#include "ConnectionController.h"
#include "FlashController.h"
#include "MeasurementController.h"
#include "TimeController.h"
#include "LedController.h"

#define BUTTON_RESET_PIN 18

TimerHandle_t thMeasurementProducer;
TaskHandle_t thMeasurementConsumer;
TaskHandle_t thLedController;
TimerHandle_t thTimerSync;
TimerHandle_t thButtonPressed;

void IRAM_ATTR buttonPressed()
{
  xTimerStart(thButtonPressed, 0);
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
  pinMode(VOLT_PIN, INPUT);
  pinMode(AMPERE_ONE_PIN, INPUT);
  pinMode(AMPERE_TWO_PIN, INPUT);
  pinMode(AMPERE_THREE_PIN, INPUT);
  attachInterrupt(BUTTON_RESET_PIN, buttonPressed, FALLING);
  Serial.println("Booting...");

  if (initializeSPIFFS())
  {
    InternetConfig* sdConf = readFromFile();
    if (sdConf != NULL)
    {
      conf = sdConf;
      Serial.println("Connecting from sd...");
      Serial.print("Username: ");
      Serial.println(conf->username);
      Serial.print("Password: ");
      Serial.println(conf->password);
      Serial.print("SSID: ");
      Serial.println(conf->ssid);
      selectEncryptionType((wifi_auth_mode_t)3, conf->ssid, conf->username, conf->password);
    }
    else
    {
      conf = new InternetConfig();
      configureDevice();
      writeToFile("/test.txt", conf);
    }
  }
  else
  {
    conf = new InternetConfig();
    configureDevice();
  }
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  client.setServer(conf->broker, 1883);
  client.connect(CLIENT_ID);

  readMeasurementFromFile();

  if (rtc.lostPower())
  {
    syncRTCtoNTP();
  }

  setupTasks();
}

void loop()
{
  vTaskDelete(NULL);
}

void onButtonPressed(TimerHandle_t xTimer)
{
  int buttonState = digitalRead(BUTTON_RESET_PIN);
  resetESP();
}

void setupTasks() {
  // ##########################
  // ### Measurements Tasks ###

  // Queue

  queue = xQueueCreate(10, sizeof(Measurement));
  if (queue == NULL)
  {
    Serial.println("Couldn't create Queue");
    ESP.restart();
  }

  // Measurement Producer
  
  thMeasurementProducer = xTimerCreate("Timer", 5000, pdTRUE, (void *)0, readTask);
  if (thMeasurementProducer == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerStartFromISR(thMeasurementProducer, (BaseType_t *)0);

  // Measurement Consumer

  xTaskCreate(valueConsumer, "consumer", 4096, (void *)1, 10, &thMeasurementConsumer);
  if (thMeasurementConsumer == NULL)
  {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }

  // ### End Measurements Tasks ###
  // ##############################

  // ####################
  // ### System Tasks ###

  // LED Controller
  
  xTaskCreate(ledTask, "consumer_light", 2048, NULL, 3, &thLedController);
  if (thLedController == NULL)
  {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }

  // RTC Time Sync to NTP

  thTimerSync = xTimerCreate("TimerSync", 3600 * 1000, pdTRUE, (void *)0, syncTime);
  if (thTimerSync == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerStart(thTimerSync, 0);

  // Reset Button

  thButtonPressed = xTimerCreate("buttonTimerPress", 1000, pdFALSE, (void *)0, onButtonPressed);
  if (thButtonPressed == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  // ### End System Tasks ###
  // ########################
}
