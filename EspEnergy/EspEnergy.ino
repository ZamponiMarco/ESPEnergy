#include "ConnectionController.h"
#include "FlashController.h"
#include "Measurement.h"
#include "TimeController.h"
#include "LedController.h"

#define BUTTON_RESET_PIN 18

TimerHandle_t timer;
TaskHandle_t taskConsumer;
TaskHandle_t consumer_light;
TimerHandle_t syncTimer;
TimerHandle_t buttonPresstTime;

const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const char *ntpServer = "pool.ntp.org";

bool sd = false;
bool wifi = false;

boolean state = false;
struct tm timeinfo;

void IRAM_ATTR buttonPressed()
{
  xTimerStart(buttonPresstTime, 0);
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
    sd = true;
    InternetConfig *sdConf = readFromFile();
    if (sdConf != NULL)
    {
      Serial.println("Connecting from sd...");
      Serial.print("Username: ");
      Serial.println(sdConf->username);
      Serial.print("Password: ");
      Serial.println(sdConf->password);
      Serial.print("SSID: ");
      Serial.println(sdConf->ssid);
      wifi = selectEncryptionType((wifi_auth_mode_t)3, sdConf->ssid, sdConf->username, sdConf->password);
    }
    else
    {
      configureDevice(conf);
      writeToFile("/test.txt", conf);
    }
  }
  else
  {
    configureDevice(conf);
  }
  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

  client.setServer(conf->broker, 1883);
  client.connect(CLIENT_ID);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if (rtc.lostPower())
  {
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return;
    }
    rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    Serial.println("Actual time:");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  xTaskCreate(ledTask, "consumer_light", 2048, NULL, 3, &consumer_light);
  if (consumer_light == NULL)
  {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }

  queue = xQueueCreate(10, sizeof(Measurement));
  if (queue == NULL)
  {
    Serial.println("Couldn't create Queue");
    ESP.restart();
  }
  timer = xTimerCreate("Timer", 5000, pdTRUE, (void *)0, readTask);
  if (timer == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerStartFromISR(timer, (BaseType_t *)0);

  xTaskCreate(valueConsumer, "consumer", 4096, (void *)1, 10, &taskConsumer);
  if (taskConsumer == NULL)
  {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }

  syncTimer = xTimerCreate("TimerSync", 3600 * 1000, pdTRUE, (void *)0, syncTime);
  if (syncTimer == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerStart(syncTimer, 0);

  buttonPresstTime = xTimerCreate("buttonTimerPress", 1000, pdFALSE, (void *)0, onButtonPressed);
  if (buttonPresstTime == NULL)
  {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }
}

void loop()
{
  vTaskDelete(NULL);
}

void syncTime(TimerHandle_t xTimer)
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime(&timeinfo);
  rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
}

void onButtonPressed(TimerHandle_t xTimer)
{
  int buttonState = digitalRead(BUTTON_RESET_PIN);
  resetESP();
}
