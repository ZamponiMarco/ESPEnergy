#ifndef captiveportal_h
#define captiveportal_h

#include <SD.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks

typedef struct {
  String ssid;
  String username;
  String password;
  String broker;
  String topic;
} Configuration;

boolean startWebServer(Configuration* input_config);
String indexPage(int num_ssid, String ssid_list[50]);
void setupNetwork();
void scanNetworks(int *num_ssid, String ssid_list[50]);
boolean selectEncryptionType(String encryption, String ssid, String username, String password);
String getEncryptionType(wifi_auth_mode_t encryptionType);
String getNetworks(int num_ssid, String ssid_list[50]);
boolean connectOpenNetwork(char* ssid);
boolean connectWpa(char* ssid, char* password);
boolean connectWpa2Enterprise(char* ssid, char* username, char* password);

#endif
