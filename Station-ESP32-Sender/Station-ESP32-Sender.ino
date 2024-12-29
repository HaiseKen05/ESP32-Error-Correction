#include <WiFi.h>



// Wi-Fi credentials
const char* ssid = "ESP32_AP";
const char* password = "12345678";


// Server IP and port
const char* serverIP = "192.168.4.1"; // Default AP IP
const uint16_t serverPort = 1234;


WiFiClient client;


// Corruption button pin
const int corruptionButtonPin = 14; // GPIO 14 (adjustable)


void setup() {
  Serial.begin(115200);


  // Initialize the corruption button pin
  pinMode(corruptionButtonPin, INPUT_PULLUP);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");


  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to Wi-Fi");


  // Connect to the Receiver (AP)
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Connected to Server");
  } else {
    Serial.println("Connection to Server Failed");
  }
}


void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    Serial.println("Message: " + message);


    // Send the Initial Message
    client.println(message);


    // Process each character for bit-by-bit transmission
    for (char c : message) {
      String binary = "";
      for (int i = 7; i >= 0; i--) {
        binary += (c & (1 << i)) ? '1' : '0';
      }


      Serial.print("Transmitting Binary of '");
      Serial.print(c);
      Serial.print("': ");


      // Send each bit
      for (int i = 0; i < 8; i++) {
        char bit = binary[i];


        // Check if the corruption button is pressed
        if (digitalRead(corruptionButtonPin) == LOW) {
          bit = (bit == '0') ? '1' : '0'; // Flip the bit
        }


        // Transmit the bit
        client.print(bit);
        Serial.print(bit); // Print in a single line
        delay(500); // Delay for visible transmission speed
      }
      Serial.println(); // End of the binary transmission line
    }
    client.print('\n'); // Signal end of bit-by-bit transmission
    Serial.println("Message Sent!");
  }
}
