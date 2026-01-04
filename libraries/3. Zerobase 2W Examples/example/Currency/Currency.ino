/*
  Currency.ino - Example code to get USD to VND exchange rate using WiFiEspAT library and open.er-api.com API
*/
#include <ZBPrint.h>

#define WIFIESPAT2
#include <WiFiEspAT.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiClient client;

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
  getUSDVND();
  delay(60000);         
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

void getUSDVND() {
  if (!client.connect("open.er-api.com", 80)) {
    Serial.println("FX connect FAIL");
    return;
  }

  client.println("GET /v6/latest/USD HTTP/1.1");
  client.println("Host: open.er-api.com");
  client.println("Connection: close");
  client.println();

  String json = "";
  bool start = false;

  while (client.connected()) {
    while (client.available()) {
      char c = client.read();
      if (c == '{') start = true;
      if (start) json += c;
    }
  }
  client.stop();

  Serial.println("RAW FX JSON:");
  Serial.println(json);

  parseUSD(json);
}

void parseUSD(String json) {
  int ratesPos = json.indexOf("\"rates\":{");
  if (ratesPos < 0) {
    Serial.println("No rates");
    return;
  }

  ratesPos += strlen("\"rates\":{");
  int ratesEnd = json.indexOf("}", ratesPos);
  String rates = json.substring(ratesPos, ratesEnd);

  int usdPos = rates.indexOf("\"VND\":");
  if (usdPos < 0) {
    Serial.println("No VND");
    return;
  }

  usdPos += strlen("\"VND\":");
  int usdEnd = rates.indexOf(",", usdPos);
  String usd = rates.substring(usdPos, usdEnd);

  Serial.print("Gia tri USD sang VND: ");
  Serial.println(usd);
}























