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
String LEDstate = "off";

unsigned long currentTime = millis();
unsigned long previousTime = 0;

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
  Serial.printf("\n\nconnected to WiFi\nconnect to server at %s:%d\n", WiFi.localIP().toString().c_str());

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
              digitalWrite(led, HIGH);
            }
            else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("LED off");
              LEDstate = "off";
              digitalWrite(led, LOW);
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
