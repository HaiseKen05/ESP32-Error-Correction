#include <WiFi.h>

// Wi-Fi credentials
const char* ssid = "ESP32_AP";
const char* password = "12345678";

// Server port
const uint16_t serverPort = 1234;

WiFiServer server(serverPort);
WiFiClient client;

const int CorrectionPin = 14;  // GPIO 14 (adjustable)

String initialMessage = "";        // To store the Initial Message
String reconstructedMessage = "";  // To store the Reconstructed Message
String currentBinary = "";         // Temporary storage for 8 bits

void setup() {
  Serial.begin(115200);

  // Initialize the button pin
  pinMode(CorrectionPin, INPUT_PULLUP);

  // Start Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
  Serial.println("Wi-Fi Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Start the server
  server.begin();
  Serial.println("Server Started");
}

void displayProgressBar() {
  Serial.print("Correcting message: ");
  for (int i = 0; i < 8; i++) {
    delay(1000);  // 1 second delay per dot
    Serial.print("X ");
    delay(750);  // Dramatic delay
  }
  Serial.println();
}

void loop() {
  // Check for new client connection
  if (!client || !client.connected()) {
    client = server.available();
    if (client) {
      Serial.println("Client Connected");
    }
  }

  if (client && client.connected() && client.available()) {
    char receivedChar = client.read();

    // Detect newline indicating end of Initial Message or Binary Data
    if (receivedChar == '\n') {
      if (initialMessage.isEmpty()) {
        // Store the Initial Message
        initialMessage = reconstructedMessage;
        reconstructedMessage = "";  // Reset for bit-by-bit reconstruction
      } else {
        // End of bit-by-bit transmission
        Serial.println("Reconstructed Message: " + reconstructedMessage);
        delay(5000);  // Delay to make it seem it is doing comparison between the initial message and reconstructed message

        // Perform comparison
        if (initialMessage != reconstructedMessage) {
          Serial.println("Discrepancy Detected!");
          Serial.println("Press the button to correct the corrupted message.");

          // Wait for button press
          while (digitalRead(CorrectionPin) == HIGH) {
            delay(10);
          }

          // Simulate correction with delay and progress bar
          displayProgressBar();

          Serial.println("Corrected Message: " + initialMessage);
        } else {
          // No discrepancy detected
          reconstructedMessage = initialMessage;  // Explicitly assign to ensure consistency
          Serial.println("No Discrepancy Detected. Final Output: " + reconstructedMessage);
        }

        // Reset for the next transmission
        reconstructedMessage = "";
      }
      return;
    }

    // Handle binary data or Initial Message
    if (initialMessage.isEmpty()) {
      // During the Initial Message phase
      reconstructedMessage += receivedChar;
    } else if (receivedChar == '0' || receivedChar == '1') {
      // During the binary transmission phase
      currentBinary += receivedChar;

      // If we have 8 bits, reconstruct the character
      if (currentBinary.length() == 8) {
        char character = 0;
        for (int i = 0; i < 8; i++) {
          if (currentBinary[i] == '1') {
            character |= (1 << (7 - i));
          }
        }
        reconstructedMessage += character;
        currentBinary = "";  // Reset for the next character
      }
    }
  }
}
