#include <ZBPrint.h>

// Define WIFIESPAT2 before including the library to enable AT2 mode
#define WIFIESPAT2
#include <WiFiEspAT.h>

// WiFi credentials
const char* ssid = "Enter your Wifi SSID";
const char* password = "Enter your Wifi password";

// Pin definitions
#define ESP_RST_PIN PA6    // RST pin connection
#define ESP_GPIO0_PIN PA7  // GPIO0 pin connection

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("Starting WiFi connection with WiFiEspAT library (AT2 mode manually enabled)...");
  Serial.println("WIFIESPAT2 defined - AT2 mode should be active");

  // Initialize ESP8285 reset pins
  pinMode(ESP_RST_PIN, OUTPUT);
  pinMode(ESP_GPIO0_PIN, OUTPUT);

  // Reset sequence for normal operation
  digitalWrite(ESP_GPIO0_PIN, HIGH);  // Set GPIO0 high for normal boot
  delay(10);

  digitalWrite(ESP_RST_PIN, LOW);  // Assert reset
  delay(50);

  digitalWrite(ESP_RST_PIN, HIGH);  // Release reset
  delay(3000);                      // Give ESP time to boot

  // Initialize ESP module on Serial4
  Serial4.begin(115200);

  // Initialize WiFiEspAT library
  WiFi.init(Serial4);

  // Check for the presence of the shield
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true) {
      delay(1000);
    }
  }

  // Print firmware version
  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware version: ");
  Serial.println(fv);

  // For AT2 firmware, enable persistent connections
  WiFi.setPersistent(true);
  delay(1000);

  // First disconnect from any existing connection
  Serial.println("Disconnecting from any existing WiFi connection...");
  WiFi.disconnect();
  delay(3000);  // Give more time for disconnection

  // Verify disconnection
  int disconnectStatus = WiFi.status();
  Serial.print("Status after disconnect: ");
  Serial.println(getStatusString(disconnectStatus));

  if (disconnectStatus == WL_CONNECTED) {
    Serial.println("Still connected, forcing disconnect...");
    WiFi.disconnect();
    delay(2000);
  }

  // Now connect to WiFi network
  Serial.print("Now connecting to WiFi network: ");
  Serial.println(ssid);

  int status = WiFi.begin(ssid, password);

  if (status == WL_CONNECTED) {
    Serial.println("Connected immediately!");
  } else {
    Serial.println("Waiting for connection (AT2 needs more time)...");

    // Wait for connection with extended timeout for AT2
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
      delay(1000);
      Serial.print(".");

      // Show status every 5 seconds
      if ((millis() - startTime) % 5000 == 0) {
        Serial.print(" [");
        Serial.print(getStatusString(WiFi.status()));
        Serial.print("]");
      }
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected successfully!");
    } else {
      Serial.println("\nWiFi connection failed!");
      Serial.print("Final status: ");
      Serial.println(getStatusString(WiFi.status()));
      Serial.println("AT2 mode is enabled but connection still failed.");
      Serial.println("This might indicate a network issue rather than library configuration.");
      return;
    }
  }

  // Print connection information
  printWifiStatus();

  Serial.println("Setup complete! WiFi is ready for use.");
}

void loop() {
  // Monitor WiFi connection status
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 30000) {  // Check every 30 seconds
    lastCheck = millis();

    int status = WiFi.status();
    if (status == WL_CONNECTED) {
      Serial.println("WiFi connection is active");
    } else {
      Serial.print("WiFi connection lost. Status: ");
      Serial.println(getStatusString(status));

      // Try to reconnect
      Serial.println("Attempting to reconnect...");
      WiFi.begin(ssid, password);

      // Wait for reconnection with AT2 timeout
      unsigned long startTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
        delay(1000);
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Reconnected successfully!");
        printWifiStatus();
      }
    }
  }

  // Your main application code goes here
  delay(1000);
}

void printWifiStatus() {
  Serial.println("\n=== WiFi Connection Information ===");

  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());

  Serial.print("DNS Server: ");
  Serial.println(WiFi.dnsIP());

  Serial.println("====================================\n");
}

String getStatusString(int status) {
  switch (status) {
    case WL_IDLE_STATUS: return "Idle";
    case WL_NO_SSID_AVAIL: return "Network not found";
    case WL_CONNECTED: return "Connected";
    case WL_CONNECT_FAILED: return "Connection failed";
    case WL_CONNECTION_LOST: return "Connection lost";
    case WL_DISCONNECTED: return "Disconnected";
    case WL_NO_MODULE: return "No module";
    default: return "Unknown (" + String(status) + ")";
  }
}