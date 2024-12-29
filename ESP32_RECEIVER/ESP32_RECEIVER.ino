#include <WiFi.h>
#include <WiFiServer.h>
#include <LiquidCrystal_I2C.h>

// Wi-Fi credentials
const char* ssid = "YourSSID";
const char* password = "YourPassword";

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Server configuration
WiFiServer server(8080);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  server.begin();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for Msg");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String message = client.readStringUntil('\n');
    Serial.println("Message Received: " + message);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    delay(2000);
  }
}
