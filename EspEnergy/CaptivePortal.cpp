#include "CaptivePortal.h"

WebServer webServer(80);
static Configuration* conf;
bool is_connected;
const char *ssidAP = "EnergyCounterESP32 AP";
int num_ssid = 0;
String ssid_list[50];

void handleRoot() {
  Serial.println("Received connection");
  webServer.send(200, "text/html", indexPage(num_ssid, ssid_list));
}

boolean startWebServer(Configuration* input_config) {
  conf = input_config;
  scanNetworks(&num_ssid, ssid_list);
  setupNetwork();
  Serial.println("Starting web server...");
  
  webServer.on("/", HTTP_GET, handleRoot);

  webServer.on("/connect", HTTP_POST, []() {
      Serial.println("Received configuration fields");
      conf->ssid = WiFi.SSID(webServer.arg("network").toInt());
      conf->username = webServer.arg("username");
      conf->password = webServer.arg("password");
      conf->broker = webServer.arg("broker");
      conf->topic = webServer.arg("topic");
      webServer.send(200, "text/plain", "Done" );
      Serial.println("Disabling access point...");
      WiFi.softAPdisconnect(true);
      String encryption = getEncryptionType(WiFi.encryptionType(webServer.arg("network").toInt()));
      Serial.println(encryption);
      is_connected = selectEncryptionType(encryption, conf->ssid, conf->username, conf->password);
    });

  webServer.begin();
  Serial.println("Web server is on.");
  while(!is_connected) {
    webServer.handleClient();
    delay(2);
  }
  return true;
}

String indexPage(int num_ssid, String ssid_list[50]) {
  String header_page = R"(
    <!DOCTYPE HTML>
    <html>
    <head> <title>ESP32 Configuration Panel</title> </head>
    <body>
    <h1>Configuration panel</h1>
    <p>Energy counter configuration panel</p>
    <form action="/connect" method="post">
      SSID:<br>
      <select name="network">
     )";

  String auth_form = R"(
        </select> <br><br>
        
        Username:<br>
        <input type="username" name="username"> *only for WPA2 Enterprise networks<br>
        Password:<br>
        <input type="password" name="password"><br><br> )";
  String footer_page = R"(
    
    <h2>Configurazione broker</h2>
    Indirizzo ip broker:
    <input type="text" name="broker"><br>
    Topic: (ES: fila/numero)<br>
    <input type="text" name="topic"><br>
    <input type="submit">
  </form>
  
  </body>
  </html>)";

  return header_page + getNetworks(num_ssid, ssid_list) + auth_form + footer_page;
}

void setupNetwork(){
  Serial.println("Starting AP mode...");
  WiFi.softAP(ssidAP);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void scanNetworks(int *num_ssid, String ssid_list[50]) {
  Serial.println("*** Scan Networks ***");
  *num_ssid = WiFi.scanNetworks();
  if ( *num_ssid == -1 ) {
    Serial.println("Nessuna rete wifi presente");
    while (true);
  }
  
  Serial.print("Reti trovate: ");
  Serial.println(*num_ssid);

  for (int thisNet = 0; thisNet < *num_ssid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    Serial.println(getEncryptionType(WiFi.encryptionType(thisNet)));
    ssid_list[thisNet] = WiFi.SSID(thisNet);
  }
}

boolean selectEncryptionType(String encryption, String ssid, String username, String password) {
  char ssidToCharArray[32];
  char usernameToCharArray[63];
  char passwordToCharArray[50];
  ssid.toCharArray(ssidToCharArray, ssid.length() + 1);
  if (encryption.equals("Open")) {
    return connectOpenNetwork(ssidToCharArray);
  }else if (encryption.equals("WPA2_Enterprise")) {
    username.toCharArray(usernameToCharArray, username.length()+1);
    password.toCharArray(passwordToCharArray, password.length()+1);
    return connectWpa2Enterprise(ssidToCharArray, usernameToCharArray, passwordToCharArray);
  }else {
    password.toCharArray(passwordToCharArray, password.length()+1);
    return connectWpa(ssidToCharArray, passwordToCharArray);
  }
}

String getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case (0):
      return "Open";
     case (1):
      return "WEP";
     case (2):
      return "WPA_PSK";
     case (3):
      return "WPA2_PSK";
     case (4):
      return "WPA_WPA2_PSK";
     case (5):
      return "WPA2_Enterprise";
     default:
      return "UNKNOWN";
  }
}

String getNetworks(int num_ssid, String ssid_list[50]) {
  String list;
  for (int i = 0; i < num_ssid; i++) {
    list += "<option value='" + String(i) + "'>" + WiFi.SSID(i) + ": " + getEncryptionType(WiFi.encryptionType(i)) + " </option>";
  }
  return list;
}

boolean connectOpenNetwork(char* ssid) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

boolean connectWpa(char* ssid, char* password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

boolean connectWpa2Enterprise(char* ssid, char* username, char* password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
  esp_wifi_sta_wpa2_ent_enable(&config); 
  WiFi.begin(ssid); //connect to wifi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}
