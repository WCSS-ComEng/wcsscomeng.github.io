#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// defines for wifi information and neopixel data
const char* ssid = "ArduinoPi";
const char* password = "WCSSroom228";
const char* serverName = "http://10.191.28.229:5000/neopixel-control";

#define PIXEL_PIN 3
#define NUM_OF_PIXELS 10

Adafruit_NeoPixel pixel(NUM_OF_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // sets up initial neopixel parameters
  Serial.begin(115200);
  pixel.begin();
  pixel.setBrightness(50);
  pixel.show();

  // connects to wifi using information set above
  WiFi.begin(ssid, password);
  Serial.print("connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nconnected!");
}

void loop() {
  // if wifi is connected, proceed. otherwise, do nothing else
  if (WiFi.status() == WL_CONNECTED) {
    
    HTTPClient http;                   // begins an http request
    http.begin(serverName);            // begins the server, which is ran through the API in ../server_tools/db_API.py
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) { // if the http response is NOT EMPTY, proceed. otherwise treat as a connection error
      // uses a StreamPtr to ensure empty payloads are not recieved
      WiFiClient* stream = http.getStreamPtr();
      size_t size = http.getSize();
      String payload = "";

      // if the size of the payload is NOT EMPTY, proceed.
      if (size > 0) {
        byte buffer[128];
        while (http.connected() && (size > 0 || size == -1)) {
          // collects information
          if (stream->available()) {
            int len = stream->readBytes(buffer, sizeof(buffer));
            payload += String((char*)buffer);
            if (size > 0) {
              size -= len;
            }
          }
          delay(1);
        }
      }

      // prints QoL and debugging information to the serial monitor.
      Serial.printf("HTTP GET Response Code: %d\n", httpResponseCode);
      Serial.printf("Payload Length: %d\n", payload.length());
      Serial.println("Payload:");
      Serial.println(payload);

      // if payload length is NOT EMPTY, proceed. otherwise, the payload is not recieved
      if (payload.length() > 0) {
        // create a JSON document for processing database information, and ensure there is NO ERRORS
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);
        // if no errors are recieved with the JSON, proceed. otherwise, assume a parsing error
        if (!error) {
          // recieves information from the API, connected using a GET method to the neopixel_control table
          int slider1 = doc["slider1"];
          int slider2 = doc["slider2"];
          String colour = doc["colour"];
          bool running = doc["running"];

          Serial.printf("slider1: %d, slider2: %d, colour: %s, running: %d\n", slider1, slider2, colour.c_str(), running);

          // convert hex colour to RGB
          long col = strtol(colour.c_str() + 1, NULL, 16); // skip the '#' character
          byte r = (col >> 16) & 0xFF;
          byte g = (col >> 8) & 0xFF;
          byte b = col & 0xFF;

          // set neopixels based on running state
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
          Serial.print("Failed to parse JSON: ");
          Serial.println(error.c_str());
        }
      } else {
        Serial.println("Received empty payload.");
      }
    } else {
      Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }
  // repeat once every 5 seconds
  delay(5000);
}
