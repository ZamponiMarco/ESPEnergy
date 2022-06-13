#pragma once 

#include <WiFi.h>
#include <WebServer.h>
#include <esp_wpa2.h>

#define SSID_AP "EnergyCounterESP32 AP"

typedef struct {
  char ssid[50];
  char username[50];
  char password[50];
  char broker[50];
  char topic[50];
} InternetConfig;

extern InternetConfig *conf;

void setupNetwork();
boolean configureDevice();
String indexPage(int num_ssid);
String getNetworks(int num_ssid);
void scanNetworks(int *num_ssid);
boolean selectEncryptionType(wifi_auth_mode_t encryption, const char* ssid, const char* username, const char* password);
void connectOpenNetwork(const char* ssid);
void connectWpa(const char* ssid, const char* password);
void connectWpa2Enterprise(const char* ssid, const char* username, const char* password);
