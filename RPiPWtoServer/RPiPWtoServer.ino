#include <WiFi.h>
#include <WiFiClient.h>

#ifndef STASSID
#define STASSID "ArduinoPi"
#define STAPSK "WCSSroom228"
#define URL "https://wcss-comeng.github.io/wcsscomeng.github.io/"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* url = URL;

int port = 8080;

WiFiServer server(port);

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("PicoW");
  Serial.printf("connecting to '%s' with '%s'\n", ssid, password);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.printf("\n\nconnected to WiFi\nconnect to server at %s:%d\n", WiFi.localIP().toString().c_str(), port);

  server.begin();

}

void loop() {

}
