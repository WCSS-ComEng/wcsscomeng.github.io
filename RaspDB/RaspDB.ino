#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "ArduinoPi";
const char* password = "WCSSroom228";
const char* serverName = "http://10.191.28.229:5000/neopixel-control";

#define PIXEL_PIN 3
#define NUM_OF_PIXELS 10

Adafruit_NeoPixel pixel(NUM_OF_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pixel.begin();
  pixel.setBrightness(50);
  pixel.show();

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

    if (httpResponseCode > 0) {
      WiFiClient* stream = http.getStreamPtr();
      size_t size = http.getSize();
      StaticJsonDocument<512> doc;

      if (size > 0) {
        DeserializationError error = deserializeJson(doc, *stream);
        if (!error) {
          int slider1 = doc["slider1"];
          int slider2 = doc["slider2"];
          String colour = doc["colour"];
          bool running = doc["running"];

          Serial.printf("speed: %d, spacing: %d, colour: %s, running: %d\n", slider1, slider2, colour.c_str(), running);

          // convert hex colour to RGB
          long col = strtol(colour.c_str() + 1, NULL, 16); // skip the '#' character
          byte r = (col >> 16) & 0xFF;
          byte g = (col >> 8) & 0xFF;
          byte b = col & 0xFF;

          // set NeoPixels based on running state
          pixel.clear();
          if (running) {
            for (int i = 0; i < NUM_OF_PIXELS; i++) {
              pixel.setPixelColor(i, r, g, b);
            }
          } else {
            for (int i = 0; i < NUM_OF_PIXELS; i++) {
              pixel.setPixelColor(i, 0, 0, 0); // off
            }
          }
          pixel.show();
        } else {
          Serial.print("failed to parse JSON: ");
          Serial.println(error.c_str());
        }
      } else {
        Serial.println("received empty payload.");
      }
    } else {
      Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(1500);
}
