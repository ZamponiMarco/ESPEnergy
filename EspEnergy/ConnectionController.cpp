#include "ConnectionController.h"

InternetConfig *conf;

WebServer webServer(80);
int num_ssid = 0;

bool is_connected;

void setupNetwork(){
  Serial.println("Starting AP mode...");
  WiFi.softAP(SSID_AP);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

boolean configureDevice() {
  scanNetworks(&num_ssid);
  setupNetwork();
  Serial.println("Starting web server...");
  
  webServer.on("/", HTTP_GET, []() {
    Serial.println("Received connection");
    webServer.send(200, "text/html", indexPage(num_ssid));
  });

  webServer.on("/connect", HTTP_POST, []() {
      Serial.println("Received configuration fields");
      WiFi.SSID(webServer.arg("network").toInt()).toCharArray(conf->ssid, 50);
      webServer.arg("username").toCharArray(conf->username, 50);
      webServer.arg("password").toCharArray(conf->password, 50);
      webServer.arg("broker").toCharArray(conf->broker, 50);
      webServer.arg("topic").toCharArray(conf->topic, 50);
      webServer.send(200, "text/plain", "Done" );
      Serial.println("Disabling access point...");
      WiFi.softAPdisconnect(true);
      is_connected = selectEncryptionType(WiFi.encryptionType(webServer.arg("network").toInt()), 
        conf->ssid, conf->username, conf->password);
    });

  webServer.begin();
  Serial.println("Web server is on.");
  while(!is_connected) {
    webServer.handleClient();
    delay(2);
  }
  return is_connected;
}

String indexPage(int num_ssid) {
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
          <input type="password" name="password">
          <br>
          <br> 
          <h2>Configurazione broker</h2>
          Indirizzo ip broker:
          <input type="text" name="broker"><br>
          Topic: (ES: fila/numero)<br>
          <input type="text" name="topic"><br>
          <input type="submit">
        </form>
  
      </body>
    </html>)";

  return header_page + getNetworks(num_ssid) + auth_form;
}

String getNetworks(int num_ssid) {
  String list;
  for (int i = 0; i < num_ssid; i++) {
    list += "<option value='" + String(i) + "'>" + WiFi.SSID(i) + ": " + WiFi.encryptionType(i) + " </option>";
  }
  return list;
}

void scanNetworks(int *num_ssid) {
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
    Serial.println(WiFi.encryptionType(thisNet));
  }
}

boolean selectEncryptionType(wifi_auth_mode_t encryption, const char* ssid, const char* username, const char* password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  switch (encryption) {
    case WIFI_AUTH_OPEN:
      connectOpenNetwork(ssid);
      break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
      connectWpa2Enterprise(ssid, username, password);
      break;
    default:
      connectWpa(ssid, password);
      break;
  }
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    //If after 30 second it doesn't connect, it calls configureDevice()
    delay(500);
    counter += 500;
    if(counter > 30000){
      return false;
    }
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Timed out");
    return false;
  }else{
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }
}

void connectOpenNetwork(const char* ssid) {
  WiFi.begin(ssid);
}

void connectWpa(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
}

void connectWpa2Enterprise(const char* ssid, const char* username, const char* password) {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(ssid);
}
