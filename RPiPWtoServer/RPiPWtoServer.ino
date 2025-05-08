#include <WiFi.h>
// #include <WiFiClient.h>

#ifndef STASSID
#define STASSID "ArduinoPi"
#define STAPSK "WCSSroom228"
#define URL "https://wcss-comeng.github.io/wcsscomeng.github.io/"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* url = URL;

const int led = LED_BUILTIN;
String header; // stores HTTP requests

WiFiServer server(8080);

void setup() {
  Serial.begin(115200);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  WiFi.mode(WIFI_STA);
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
  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;

    Serial.println("new client");

    String currentLint = "";

    while (client.connected() && currentTime - previousTime <= 2000) {
      currentTime = millis();

      if (client.available()) {
        // HTTP requests go here
      }
    }
  }
}
