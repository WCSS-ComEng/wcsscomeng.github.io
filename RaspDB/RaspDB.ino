#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

// wifi and API information
const char* ssid = "ArduinoPi";
const char* password = "WCSSroom228";
const char* serverName = "http://10.191.28.229:5000/neopixel-control";

// neopixel information
#define PIXEL_PIN 3
#define NUM_OF_PIXELS 10

Adafruit_NeoPixel pixel(NUM_OF_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// placeholders for inputs on the website
int slider1 = 50;  // speed
int slider2 = 5;   // spacing
uint8_t r, g, b;
bool running = false;

// animation variables
int currentPosition = 0;
unsigned long lastUpdate = 0;
unsigned long interval = 100; // default speed delay in ms

void setup() {
  Serial.begin(115200);
  pixel.begin();
  pixel.setBrightness(50);
  pixel.show();

  // connects to the wifi, also handles reconnecting if wifi connection is lost
  WiFi.begin(ssid, password);
  Serial.print("connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nconnected!");
}

// function to keep pixels the same if the running toggle is off, STATIC
void updatePixelsStatic() {
  // all pixels lit with color, no animation
  pixel.clear();
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    pixel.setPixelColor(i, r, g, b);
  }
  pixel.show();
}

// function to process the animation if running toggle is on, RUNNING
void updatePixelsRunning() {
  pixel.clear();

  // draws running pixels with spacing
  for (int i = 0; i < NUM_OF_PIXELS; i++) {
    // light every (spacing + 1) pixel with offset currentPosition
    if ((i + currentPosition) % (slider2 + 1) == 0) {
      pixel.setPixelColor(i, r, g, b);
    }
  }
  pixel.show();

  // advance position
  currentPosition++;
  if (currentPosition > slider2) currentPosition = 0;
}

// ensures the API is not overloaded with HTTP requests, instead only fetching new data every 5 seconds
unsigned long lastFetch = 0;              // for tracking last data fetch
const unsigned long fetchInterval = 5000; // fetch every 5 seconds

void loop() {
  unsigned long now = millis();

  // fetch new settings every 5 seconds, prevents HTTP request overload
  if (WiFi.status() == WL_CONNECTED && (now - lastFetch >= fetchInterval)) {
    lastFetch = now;
    
    // initializes HTTP request information
    HTTPClient http;
    http.begin(serverName);
    http.setTimeout(1000);  // set timeout to 1 second
    int httpResponseCode = http.GET();

    // makes sure the response code is NOT 0 (NOT EMPTY)
    if (httpResponseCode > 0) {
      WiFiClient* stream = http.getStreamPtr();
      size_t size = http.getSize();
      // creates a JSON "doc" to parse data to/from the API
      StaticJsonDocument<512> doc;

      // makes sure the data sent to/from the API is NOT empty
      if (size > 0) {
        DeserializationError error = deserializeJson(doc, *stream);
        if (!error) {
          // if there is not parsing error, fetch data from the API, which is itself connected to the database
          slider1 = doc["slider1"];
          slider2 = doc["slider2"];
          String colour = doc["colour"];
          running = doc["running"];

          // debugging information on the pico
          Serial.printf("speed: %d, spacing: %d, colour: %s, running: %d\n", slider1, slider2, colour.c_str(), running);

          // convert hex colour to RGB
          long col = strtol(colour.c_str() + 1, NULL, 16);
          r = (col >> 16) & 0xFF;
          g = (col >> 8) & 0xFF;
          b = col & 0xFF;

          // update animation interval and spacing
          interval = map(slider1, 0, 100, 500, 10);
          if (slider2 < 1) slider2 = 1;

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
  }

  // update animation
  if (running) {
    if (now - lastUpdate >= interval) {
      updatePixelsRunning();
      lastUpdate = now;
    }
  } else {
    updatePixelsStatic();
  }

  delay(5);  // ensures smooth animation
}
