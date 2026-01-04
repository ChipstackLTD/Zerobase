/*
  EchoTCP.ino - Example code to perform TCP echo test using WiFiEspAT library and tcpbin.com port 4242
  This example connects to a TCP echo server, sends a message, and prints the echoed response.
*/
#include <ZBPrint.h>

#define WIFIESPAT2
#include <WiFiEspAT.h>
#include <WiFiUdp.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiClient tcpClient;

const char* tcpHost = "tcpbin.com";
const int tcpPort = 4242;

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial4.begin(115200);
  WiFi.init(Serial4);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Cannot connect to WiFi module");
  }

  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware version: ");
  Serial.println(fv);

  WiFi.setPersistent(true);
  delay(1000);

  Serial.println("Disconnect to all WiFi...");
  WiFi.disconnect();
  delay(3000);

  Serial.print("Connect to WiFi: ");
  Serial.println(ssid);

  int status = WiFi.begin(ssid, password);
  delay(2000);
  if (status == WL_CONNECTED) {
    Serial.println("Connect to WiFi successfully");
  } else {
    Serial.println("Connect to WiFi failed");
  }

  printWifiStatus();
}

void loop() {
  tcpEchoTest();
  delay(60000);         // 1 phút lấy 1 lần
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());

  Serial.print("DNS Server: ");
  Serial.println(WiFi.dnsIP());
}

void tcpEchoTest() {
  Serial.println("TCP connect...");

  if (!tcpClient.connect(tcpHost, tcpPort)) {
    Serial.println("TCP CONNECT FAIL");
    return;
  }

  Serial.println("TCP CONNECT OK");

  // Gửi dữ liệu
  tcpClient.print("hello from arduino\r\n");

  // Đợi server echo
  unsigned long t = millis();
  while (tcpClient.connected() && millis() - t < 3000) {
    while (tcpClient.available()) {
      char c = tcpClient.read();
      Serial.write(c);
    }
  }

  tcpClient.stop();
  Serial.println("\nTCP CLOSED");
}

















