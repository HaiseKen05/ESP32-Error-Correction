#include <WiFi.h>
#include <WiFiClient.h>

// Wi-Fi credentials
const char* ssid = "YourSSID";
const char* password = "YourPassword";

// Server IP and Port
const char* serverIP = "192.168.1.100"; // Replace with Receiver's IP
const uint16_t serverPort = 8080;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    if (client.connect(serverIP, serverPort)) {
      client.println(message);
      Serial.println("Message Sent: " + message);
      client.stop();
    } else {
      Serial.println("Failed to connect to receiver");
    }
  }
}
