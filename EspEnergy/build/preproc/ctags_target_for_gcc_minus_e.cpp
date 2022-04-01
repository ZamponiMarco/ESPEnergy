# 1 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
# 2 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2
# 3 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2
# 4 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2

# 6 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2
# 7 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2

# 9 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2
# 10 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2
# 11 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2
# 12 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 2

#define VOLT_PIN 35
#define AMPERE_ONE_PIN 32
#define AMPERE_TWO_PIN 33
#define AMPERE_THREE_PIN 25
#define BUTTON_RESET_PIN 18

//#define DEBUG

TimerHandle_t timer;
QueueHandle_t queue;
TaskHandle_t taskConsumer;
BaseType_t xHigherPriorityTaskWoken = ( ( BaseType_t ) 0 );
TaskHandle_t consumer_light;

const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

RTC_DS3231 rtc;

static InternetConfig* conf = new InternetConfig();

char *clientID = "Building0Test";
WiFiClient espClient;
PubSubClient client(espClient);

bool sd = false;
bool wifi = false;

boolean state = false;

void __attribute__((section(".iram1" "." "16"))) buttonPressed(){
  state = true;
}

void setup()
{
  Serial.begin(115200);
  manageLed();
  pinMode(18, 0x05);
  pinMode(35, 0x01);
  pinMode(32, 0x01);
  pinMode(33, 0x01);
  pinMode(25, 0x01);
  attachInterrupt(18, buttonPressed, 0x02);
  Serial.println("Booting...");

  if (initializeSPIFFS()) {
    sd = true;
    InternetConfig* sdConf = readFromFile();
    if (sdConf != 
# 62 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 3 4
                 __null
# 62 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
                     ) {
      Serial.println("Connecting from sd...");
      Serial.print("Username: ");
      Serial.println(sdConf->username);
      Serial.print("Password: ");
      Serial.println(sdConf->password);
      Serial.print("SSID: ");
      Serial.println(sdConf->ssid);
      selectEncryptionType((wifi_auth_mode_t) 3, sdConf->ssid, sdConf->username, sdConf->password);
      wifi = true;
    } else {
      configureDevice(conf);
      writeToFile("/test.txt", conf);
    }
  } else {
    configureDevice(conf);
  }

  rtc.begin();


  client.setServer(conf->broker, 1883);
  client.connect(clientID);
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(((reinterpret_cast<const __FlashStringHelper *>(("Apr  1 2022")))), ((reinterpret_cast<const __FlashStringHelper *>(("18:27:58"))))));
  }
  Serial.println("Connected to MQTT broker");

  queue = xQueueGenericCreate( ( 10 ), ( sizeof(Measurement) ), ( ( ( uint8_t ) 0U ) ) );
  if (queue == 
# 91 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 3 4
              __null
# 91 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
                  ) {
    Serial.println("Couldn't create Queue");
    ESP.restart();
  }
  timer = xTimerCreate("Timer", 5000, ( ( BaseType_t ) 1 ), (void *)0, readTask);
  if (timer == 
# 96 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 3 4
              __null
# 96 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
                  ) {
    Serial.println("Couldn't create Timer");
    ESP.restart();
  }

  xTimerGenericCommand( ( timer ), ( ( BaseType_t ) 6 ), ( xTaskGetTickCountFromISR() ), ( (BaseType_t*)0 ), 0U );

  xTaskCreate(valueConsumer, "consumer", 4096, ( void * ) 1, ( ( UBaseType_t ) 0U ), &taskConsumer);
  if (taskConsumer == 
# 104 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 3 4
                     __null
# 104 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
                         ) {
    Serial.println("Couldn't create Task");
    ESP.restart();
  }

}

void loop()
{
# 127 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
}

void manageLed(){
  setupLed();
  xTaskCreate(ledTask, "consumer_light", 2048, 
# 131 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 3 4
                                              __null
# 131 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
                                                  , 3, 
# 131 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 3 4
                                                       __null
# 131 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
                                                           );
}

void ledTask(void * parameter){
  byte busStatus = Wire.endTransmission();
  // Repeat every second
  for(;;){
    if(WiFi.status() == WL_CONNECTED){
      setGreenLight();
    }
    if(WiFi.status() == WL_CONNECTED && queue != 
# 141 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino" 3 4
                                                __null
# 141 "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino"
                                                    ){
      turnOffLed();
      delay(200);
      setBlueLight();
    }
    if(WiFi.status() == WL_CONNECTED && busStatus==0){
      setRedLight();
      delay(200);
      setBlueLight();
    }
    if(WiFi.status() == WL_DISCONNECTED){
      setRedLight();
    }
    delay(1000);
  }
}

void readTask(TimerHandle_t xTimer) {
  Serial.println(state);
  int buttonState = digitalRead(18);
  Serial.println(buttonState);
  if(state){
    resetESP();
  }
  int volt = analogRead(35);
  int ampere_one = analogRead(32);
  int ampere_two = analogRead(33);
  int ampere_three = analogRead(25);
  DateTime now = rtc.now();
  printDateTime(now);
  Measurement misura = {volt, ampere_one, ampere_two, ampere_three, now};
  if (xQueueGenericSendFromISR( ( queue ), ( &misura ), ( &xHigherPriorityTaskWoken ), ( ( BaseType_t ) 0 ) )) {
    Serial.println("Object sent");
  } else {
    Serial.println("Queue is full");
  }
}

void valueConsumer(void *pvParameters)
{
  if (!(( ( uint32_t ) pvParameters ) == 1)) { ets_printf("%s:%d (%s)- assert failed!\n", "c:\\Users\\User\\Desktop\\Repository GitHub\\ESPEnergy\\EspEnergy\\EspEnergy.ino", 181, __FUNCTION__); abort(); };
  Measurement received;
  for (;;)
  {
    if (xQueueGenericReceive( ( queue ), ( &received ), ( ( TickType_t ) 100 ), ( ( BaseType_t ) 0 ) ) == ( ( ( BaseType_t ) 1 ) ))
    {
      sendMqttData(received);
    }
  }
}

void sendMqttData(Measurement measurement) {
  const char* topic = conf->topic;
  if (client.connected() || client.connect(clientID)) {
    Serial.print("Topic is: ");
    Serial.print(conf->topic);
    if (client.publish(topic, toJson(measurement).c_str())) {
      Serial.println(" Publish ok");
      setGreenLight();
    }
    else {
      Serial.println(" Publish failed");
    }
  } else{
  }
}
