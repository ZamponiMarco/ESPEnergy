#include <WiFi.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>

#include "include/Consumption.h"
#include "include/Impulse.h"
#include "include/ManageTime.h"
#include "include/DataLogger.h"
#include "include/CaptivePortal.h"
#include "include/RGBLed.h"

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

RTC_DS1307 rtc;
AsyncWebServer webServer(80);
int num_ssid = 0;
String ssid_list[50];
volatile boolean is_connected = false;
volatile boolean setup_completed = false;
volatile boolean sdcard_inserted = false;

// =================== INTERRUPTS E CONSUMI =======================
int i,j,k;
unsigned long prev_millis;
int toll = 0;
DateTime cur_ts;
bool consumption_b;

struct impulse impulseOne[300];
int indexImpulseOne = 0;
struct consumption consumptionOne[300];
int indexConsumptionOne = 0;

unsigned long startTimeOne = 0;
unsigned long endTimeOne = 0;
unsigned long deltaTimeOne = 0;

volatile bool flag_reset = false;
volatile bool flag_ap1 = false;

void IRAM_ATTR ap_reset(){
  flag_reset = true;
}

void IRAM_ATTR ap1_int() {
  // Fetch current clock time
  endTimeOne = millis();

  // Calculate time difference between start and end
  deltaTimeOne = (endTimeOne - startTimeOne);

  // If time difference bigger than 1 second
  if (deltaTimeOne > 1000) {

    // Put in the next array cell of impulses the time difference
    impulseOne[indexImpulseOne].dur = deltaTimeOne;

    // The current end time becomes the next start time
    startTimeOne = endTimeOne;
    flag_ap1 = true;
  }
}

// =================== FINE INTERRUPTS E CONSUMI =======================

// Topic MQTT
String ssid;
String username;
String password;
String WiFiEnc;
char mqtt_server[15];
char *clientID = "Building0Test";
char outTopic_Ap1[10];
char outTopic_Ap2[10];
char outTopic_Ap3[10];
char outTopic_Ap4[10];
WiFiClient espClient;
PubSubClient client(espClient);

boolean startWebServer() {
  Serial.println("Starting web server...");
  
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", indexPage(num_ssid, ssid_list) );
  });
  
  webServer.on("/connect", HTTP_POST, [] (AsyncWebServerRequest * request) {
    int params = request->params();
    String wifi_data[params];
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);

      if (p->isPost()) {
        wifi_data[i] = p->value().c_str();
      }
    }

    String ssid_to_string = wifi_data[0].substring(1, wifi_data[0].length());
    String encryption;
    
    for (int i = 0; i < num_ssid; i++) {
      if (ssid_list[i] == ssid_to_string.substring(0, ssid_to_string.length()-1)) {
        encryption = getEncryptionType(WiFi.encryptionType(i));
        break;
      }
    }
    WiFiEnc = encryption;
    wifi_data[3].toCharArray(mqtt_server, wifi_data[3].length()+1);
    wifi_data[4].toCharArray(outTopic_Ap1, wifi_data[4].length()+1);
    wifi_data[5].toCharArray(outTopic_Ap2, wifi_data[5].length()+1);
    wifi_data[6].toCharArray(outTopic_Ap3, wifi_data[6].length()+1);
    wifi_data[7].toCharArray(outTopic_Ap4, wifi_data[7].length()+1);

    ssid = ssid_to_string.substring(0, wifi_data[0].length()-1);
    username = wifi_data[1];
    password = wifi_data[2];

    Serial.println("Disabling access point...");
    WiFi.softAPdisconnect(true);
    is_connected = selectEncryptionType(encryption, ssid, wifi_data[1], wifi_data[2]);
    request->send(200, "text/plain", "Connessione alla rete in corso...<br /><br />Modalità AP disabilitata." );
  });
  webServer.begin();
  Serial.println("Web server is on.");
  return true;
}

// Defines the methods to call when certin buttons are pressed
void defineInterrupts(){
  pinMode(35, INPUT);
  pinMode(34, INPUT);
  attachInterrupt(digitalPinToInterrupt(35), ap1_int, FALLING);
  attachInterrupt(digitalPinToInterrupt(34), ap_reset, FALLING);
}

// Disables the methods registered before
void disableInterrupts(){
  detachInterrupt(digitalPinToInterrupt(35));
  detachInterrupt(digitalPinToInterrupt(34));
}

// Called when reset interrupt is called
void reset(){
  Serial.println("Deleting configuration file...");
  if(searchConfigurationFile())
    SD.remove("/connection.config");
  
  SD.remove("/consumptions_data.txt");
  Serial.println("Restarting...");
  delay(1000);
  ESP.restart();
}

void setupRoutines(){
  Serial.println("Starting setup...");
  setupRTC(&rtc);
  syncTimeWithNTP(&rtc);

  client.setServer(mqtt_server, 1883);
  Serial.print("ORA RTC: ");
  Serial.println(rtc.now().timestamp());

  defineInterrupts();
  xTaskCreate (
    setTimeInt,     // Function that should be called
    "Task1",        // Name of the task (for debugging)
    20000,          // Stack size (bytes)
    NULL,           // Parameter to pass
    1,              // Task priority
    NULL            // Task handle
  );
  setup_completed = true;
  Serial.println("Setup completed");
}

void runTask2(void * parameter ){
  for(;;){
    if(is_connected && !setup_completed){
      setupRoutines();
    }
    if(setup_completed){
      if(sdcard_inserted){
        saveSettingsToSdCard(WiFiEnc, ssid, username, password, mqtt_server, outTopic_Ap1, outTopic_Ap2, outTopic_Ap3, outTopic_Ap4);
      }
      
      vTaskDelete(Task2);
    }
  }
}

void loadSettingsFromSDCard(){
  int i = 0;
  String configurationData[9];
  File configFile = SD.open("/connection.config", FILE_READ);
  if(configFile.available()){
    while(configFile.available()){
      configurationData[i] = configFile.readStringUntil('\n');
      i++;
    }
    ssid = configurationData[0];
    username = configurationData[1];
    password = configurationData[2];
    WiFiEnc = configurationData[3];
    configurationData[4].toCharArray(mqtt_server, configurationData[4].length());
    configurationData[5].toCharArray(outTopic_Ap1, configurationData[5].length());
    configurationData[6].toCharArray(outTopic_Ap2, configurationData[6].length());
    configurationData[7].toCharArray(outTopic_Ap3, configurationData[7].length());
    configurationData[8].toCharArray(outTopic_Ap4, configurationData[8].length());
    configFile.close();
  }
}

void manageLed(){
  setupLed();
  xTaskCreate(ledTask, "Task3", 2048, NULL, 3, NULL);
}

void ledTask(void * parameter){
  // Repeat every second
  for(;;){

    // Setup non completato => BLUE
    if(!setup_completed){
      setBlueLight();
    }
    
    // Setup completato e connessione WiFi => GREEN
    if(setup_completed && WiFi.status() == WL_CONNECTED){
      setGreenLight();
    }

    // WiFi Connected and setup not completed => BLUE
    if(WiFi.status() == WL_CONNECTED && !setup_completed){
      turnOffLed();
      delay(200);
      setBlueLight();
    }

    // WiFi Connected, Setup not completed, RTC not running => RED + BLUE
    if(WiFi.status() == WL_CONNECTED && !setup_completed && !rtc.isrunning()){
      setRedLight();
      delay(200);
      setBlueLight();
    }

    // WiFi not connected => RED
    if(setup_completed && WiFi.status() == WL_DISCONNECTED){
      setRedLight();
      WiFi.reconnect();
    }

    // Setup completed and RTC not running => RED + GREEN
    if(setup_completed && !rtc.isrunning()){
      setRedLight();
      delay(300);
      setGreenLight();
    }
    
    delay(1000);
  }
}

void setTimeInt( void * parameter ) {
  for (;;) {
    if (flag_ap1) {
      Serial.println("interrupt 1");
      impulseOne[indexImpulseOne].t = rtc.now();
      indexImpulseOne++;
      flag_ap1 = false;
    }

    if(flag_reset){
      Serial.println("Resetting...");
      disableInterrupts();
      reset();
    }
  }
}

void sendMqttData(char* topic, consumption consumi[], int *dim){
  char buffer0[30];
  String payload;
  int i=0;
  client.connect(clientID);
  
  Serial.println("Invio vettori in corso...");
  for(int j=0; j<*dim; j++) {
    if(consumi[j].sent==0){
      dtostrf(consumi[j].w,4,0,buffer0);
      payload=getTimestamp(consumi[j].t)+"_"+buffer0;
      payload.toCharArray(buffer0,30);
      if (client.connected()) {
        client.publish(topic, buffer0);
        Serial.print("MQTT DATA: ");
        Serial.print(topic);
        Serial.print(" -> ");
        Serial.println(payload);
        delay(100);
      }else {
        Serial.println("Non riesco a ricollegarmi.");
        if(sdcard_inserted){
          writeConsumptionToFile(topic, buffer0); 
        }
      }
      consumptionOne[j].sent=true;
     }
    }
    *dim=0;
}

void convertToWatt(impulse *impulsi, consumption consumi[], int *dimI, int *dimC) {
  if(*dimI == 0){
    Serial.println("Impulsi a vuoto carico!");
    consumi[0].w=0;
    consumi[0].t = rtc.now();
    *dimC=1; 
  } else{
      j=0;
      prev_millis=impulsi[0].dur;
      cur_ts=impulsi[0].t;
      
      for(i=1; i < *dimI; i++){
          consumi[j].t=cur_ts;
          consumi[j].w=3600*1000/prev_millis;
          consumi[j].sent=false;
          j++;
          cur_ts=impulsi[i].t;
          prev_millis=impulsi[i].dur;
      }
      
      impulsi[0].dur =impulsi[*dimI-1].dur;
      impulsi[0].t   =impulsi[*dimI-1].t;
      *dimI=1;
      *dimC=j;
    }
}

void setup() { 
  Serial.begin(115200);
  // Start LED manager task
  manageLed();

  // Checks if SD card is inserted
  sdcard_inserted = setupDataLogger();

  // If configuration file exists
  if(searchConfigurationFile()){
    Serial.println("Configuration file found\nLoading settings from SD card...");
    // Loads settings from SD
    loadSettingsFromSDCard();

    // Tries to connect to WiFi
    is_connected = selectEncryptionType(WiFiEnc.substring(0, WiFiEnc.length()-1), ssid.substring(0, ssid.length()), username.substring(0, username.length()-1),
      password.substring(0, password.length()-1));
    
    // Setup routines
    setupRoutines();
  }
  // If configuration file does not exist
  else{
    Serial.println("Configuration file doesn't exists\nStarting captive portal...");
    if(!sdcard_inserted){
      Serial.println("SD card not detected, the configuration will not be saved.");
    }
    scanNetworks(&num_ssid, ssid_list);
    setupNetwork();
    startWebServer();
    xTaskCreate (runTask2, "Task2", 10000, NULL, 2, NULL);
  }
}

void loop() {
  // Every 3 minutes
  delay(180000);
  if(is_connected && setup_completed){
    convertToWatt(impulseOne, consumptionOne, &indexImpulseOne, &indexConsumptionOne);
  
    sendMqttData(outTopic_Ap1, consumptionOne, &indexConsumptionOne);

    if(sdcard_inserted)
      resendBackupData(&client, clientID);
  }
}
