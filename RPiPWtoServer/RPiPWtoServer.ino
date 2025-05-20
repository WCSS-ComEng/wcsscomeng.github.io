#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
// #include <WiFiClient.h>

#ifndef STASSID
#define STASSID "ArduinoPi"
#define STAPSK "WCSSroom228"
#define URL "https://wcss-comeng.github.io/wcsscomeng.github.io/"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* url = URL;

// connected to PIN 6 / GPIO 3
#define PIXEL_PIN 3
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


//const int led = 1;
String header; // stores HTTP requests
String LEDstate = "on";

unsigned long currentTime = millis();
unsigned long previousTime = 0;

WiFiServer server(8080);

void setup() {
  Serial.begin(115200);
  pixel.begin();
  delay(10);

  pixel.setBrightness(50);
  pixel.setPixelColor(0, 255, 255, 255);
  pixel.show();

  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.mode(WIFI_STA);
  Serial.printf("connecting to '%s' with '%s'\n", ssid, password);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  
  Serial.printf("\n\nconnected to WiFi\nconnect to server at %s:%d\n", WiFi.localIP().toString().c_str(), 8080);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;

    Serial.println("new client");

    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= 2000) {
      currentTime = millis();

      if (client.available()) {
        // HTTP requests go here
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("content-type: text/html");
            client.println("connection: close");
            client.println();

            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("LED on");
              LEDstate = "on";
                digitalWrite(LED_BUILTIN, HIGH);
                pixel.setPixelColor(0, 255, 255, 255);
                pixel.show();
            }
            else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("LED off");
              LEDstate = "off";
                digitalWrite(LED_BUILTIN, LOW);
                pixel.setPixelColor(0, 0, 0, 0);
                pixel.show();
            }
            // HTML webpage
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // CSS styling
            client.println("<style>html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #800080; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #000000;}</style></head>");

            // webpage heading
            client.println("<body><h1>Pico W LED Web Control Switch</h1>");

            // displaying led state
            client.println("<p>LED state is " + LEDstate + "</p>");

            // set buttons
            if (LEDstate == "off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("</body></html>");
            client.println(); 
            break;
          }
          else {
            currentLine = "";  
          }
        }
        else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("client disconnected");
    Serial.println();
  }
}
