#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// WiFi credentials
const char* ssid = "ArduinoPi";
const char* password = "WCSSroom228";

const char* serverName = "http://10.191.28.229:5000/button-state";

// useful code in POWERSHELL: py C:\Users\gsorg1\Documents\GitHub\wcsscomeng.github.io\server_tools\db_api.py

#define PIXEL_PIN 3
#define NUM_OF_PIXELS 10

Adafruit_NeoPixel pixel(NUM_OF_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pixel.begin();
  pixel.setBrightness(50);
  pixel.show();  // initialize all pixels as off

// connects to wifi
  WiFi.begin(ssid, password);
  Serial.print("connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nconnected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.printf("Payload length: %d\n", payload.length());
      Serial.println(payload);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        int state = doc["state"];
        Serial.printf("Button state: %d\n", state);

        // Set NeoPixels based on state
        pixel.clear();
        for (int i = 0; i < NUM_OF_PIXELS; i++) {
          if (state == 1) {
            pixel.setPixelColor(i, 255, 255, 255); // on
          } else {
            pixel.setPixelColor(i, 0, 0, 0);       // off
          }
        }
        pixel.show();

      } else {
        Serial.println("failed to parse JSON.");
      }

    } else {
      Serial.printf("HTTP GET failed, code: %d\n", httpResponseCode);
    }
    http.end();

  } else {
    Serial.println("WiFi disconnected");
  }

  delay(5000);
}