#ifndef captiveportal_h
#define captiveportal_h

#include <WiFi.h>
#include <WebServer.h>
#include <esp_wpa2.h>

typedef struct {
  String ssid;
  String username;
  String password;
  String broker;
  String topic;
} Configuration;

void setupNetwork();
boolean configureDevice(Configuration* input_config);
String indexPage(int num_ssid);
String getNetworks(int num_ssid);
void scanNetworks(int *num_ssid);
boolean selectEncryptionType(wifi_auth_mode_t encryption, String ssid, String username, String password);
void connectOpenNetwork(const char* ssid);
void connectWpa(const char* ssid, const char* password);
void connectWpa2Enterprise(const char* ssid, const char* username, const char* password);

#endif
