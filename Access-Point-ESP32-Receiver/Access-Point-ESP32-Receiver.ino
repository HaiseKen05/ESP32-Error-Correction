#include <WiFi.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h>
// Wi-Fi credentials
const char* ssid = "ESP32_AP";
const char* password = "12345678";

// Server port
const uint16_t serverPort = 1234;

WiFiServer server(serverPort);
WiFiClient client;

// LCD settings
hd44780_I2Cexp lcd(0x27, 20, 4); // Change 0x27 to your LCD's I2C address if needed

String initialMessage = "";         // To store the Initial Message
String reconstructedMessage = "";   // To store the Reconstructed Message
String currentBinary = "";          // Temporary storage for 8 bits

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.print("Starting...");
  delay(2000);

  // Start Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
  Serial.println("Wi-Fi Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  lcd.clear();
  lcd.print("Wi-Fi Started");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.softAPIP());

  // Start the server
  server.begin();
  Serial.println("Server Started");
  lcd.clear();
  lcd.print("Server Ready");
}

void displayProgressBar() {
  Serial.print("Correcting message: ");
  lcd.clear();
  lcd.print("Correcting...");

  for (int i = 0; i < 7; i++) {
    delay(1000); // 1 second delay per dot
    Serial.print("X ");
    lcd.setCursor(i, 1); // Update progress on LCD
    lcd.print("X ");
  }
  Serial.println();
}

void loop() {
  // Check for new client connection
  if (!client || !client.connected()) {
    client = server.available();
    if (client) {
      Serial.println("Client Connected");
      lcd.clear();
      lcd.print("Client Connected");
    }
  }

  if (client && client.connected() && client.available()) {
    char receivedChar = client.read();

    // Detect newline indicating end of Initial Message or Binary Data
    if (receivedChar == '\n') {
      if (initialMessage.isEmpty()) {
        // Store the Initial Message (trimmed)
        initialMessage = reconstructedMessage;
        initialMessage.trim(); // Remove trailing spaces or newlines
        reconstructedMessage = ""; // Reset for bit-by-bit reconstruction
      } else {
        // End of bit-by-bit transmission
        reconstructedMessage.trim(); // Remove trailing spaces or newlines
        Serial.println("Reconstructed Message: " + reconstructedMessage);
        lcd.clear();
        lcd.print("Received Message: ");
        lcd.setCursor(0,1);
        lcd.print(reconstructedMessage);
        delay(5000); // Dramatic delay 
        
        // Perform comparison
        if (initialMessage != reconstructedMessage) {
          Serial.println("Discrepancy Detected!");
          lcd.clear();
          lcd.print("Discrepancy!");
          delay(2000);
          displayProgressBar();

          Serial.println("Corrected Message: " + initialMessage);
          lcd.clear();
          lcd.print("Corrected:");
          lcd.setCursor(0, 1);
          lcd.print(initialMessage);
        } else {
          // No discrepancy detected
          Serial.println("No Discrepancy Detected. Final Output: " + reconstructedMessage);
          lcd.clear();
          lcd.print("Final Msg:");
          lcd.setCursor(0, 1);
          lcd.print(reconstructedMessage);
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
        currentBinary = ""; // Reset for the next character
      }
    }
  }
}
