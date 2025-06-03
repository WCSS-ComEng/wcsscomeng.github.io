#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Adafruit_NeoPixel.h>
#include "secrets.h" // contains wifiSSID, wifiPSK, SQLuser, SQLpassword, SQLHost, SQLdatabase, SQLPort

#define PIXEL_PIN 3
#define NUM_OF_PIXELS 10
Adafruit_NeoPixel pixel(NUM_OF_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// convert the SQLHost string to an IPAddress object
IPAddress server_addr;

WiFiClient client;
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

// mutable copies of credentials
char user[20];
char password[20];
char database[30];

void setup() {
  Serial.begin(115200);
  pixel.begin();
  pixel.setBrightness(50);

  // prepare mutable credential copies
  strncpy(user, SQLuser, sizeof(user));
  user[sizeof(user) - 1] = '\0';

  strncpy(password, SQLpassword, sizeof(password));
  password[sizeof(password) - 1] = '\0';

  strncpy(database, SQLdatabase, sizeof(database));
  database[sizeof(database) - 1] = '\0';

  // convert SQLHost string to IPAddress
  if (!server_addr.fromString(SQLHost)) {
    Serial.println("Invalid IP address in SQLHost");
    while (true) delay(1000); // Halt if IP invalid
  }

  // connects to wifi
  WiFi.begin(wifiSSID, wifiPSK);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // connects to MySQL
  Serial.println("Connecting to MySQL...");
  if (conn.connect(server_addr, SQLPort, user, password, database)) {
    Serial.println("MySQL Connected.");
  } else {
    Serial.println("MySQL Connection failed.");
  }

  cursor = new MySQL_Cursor(&conn);
}

void loop() {
  if (conn.connected()) {
    cursor->execute("SELECT state FROM button_values ORDER BY last_modified DESC LIMIT 1;");

    row_values *row = cursor->get_next_row();
    if (row != nullptr) {
      int state = atoi(row->values[0]);
      Serial.printf("Button state from DB: %d\n", state);

      for (int i = 0; i < NUM_OF_PIXELS; i++) {
        if (state == 1) {
          pixel.setPixelColor(i, 255, 255, 255);
        } else {
          pixel.setPixelColor(i, 0, 0, 0);
        }
      }
      pixel.show();
    } else {
      Serial.println("No rows received from query.");
    }

  } else {
    Serial.println("MySQL disconnected. Attempting to reconnect...");
    if (conn.connect(server_addr, SQLPort, user, password, database)) {
      Serial.println("MySQL Reconnected.");
    }
  }

  delay(5000);
}
