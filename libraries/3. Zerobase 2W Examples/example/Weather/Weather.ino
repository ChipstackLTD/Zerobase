/*
  Weather.ino - Example code to get current weather in Da Nang using WiFiEspAT library and open-meteo.com API
  This code connects to a WiFi network and retrieves the current temperature in Da Nang.
*/
#include <ZBPrint.h>

#define WIFIESPAT2
#include <WiFiEspAT.h>

const char* ssid = "Quang Hung";
const char* password = "quanghung68";

WiFiClient client;

const char* weatherHost = "api.open-meteo.com";
const int   weatherPort = 80;

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
  getWeatherDaNang();   // lấy thời tiết
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

void getWeatherDaNang() {
  if (!client.connect("api.open-meteo.com", 80)) {
    Serial.println("Weather connect FAIL");
    return;
  }

  client.println(
    "GET /v1/forecast?"
    "latitude=16.0544&longitude=108.2022"
    "&current_weather=true HTTP/1.1"
  );
  client.println("Host: api.open-meteo.com");
  client.println("Connection: close");
  client.println();

  String payload = "";
  bool jsonStart = false;

  while (client.connected()) {
    while (client.available()) {
      char c = client.read();
      if (c == '{') jsonStart = true;
      if (jsonStart) payload += c;
    }
  }

  client.stop();

  Serial.println("RAW JSON:");
  Serial.println(payload);

  parseTemperature(payload);
}


void parseTemperature(String json) {
  int cwStart = json.indexOf("\"current_weather\":{");
  if (cwStart < 0) {
    Serial.println("No current_weather");
    return;
  }

  cwStart += strlen("\"current_weather\":{");
  int cwEnd = json.indexOf("}", cwStart);
  String cw = json.substring(cwStart, cwEnd);

  int tIdx = cw.indexOf("\"temperature\":");
  if (tIdx < 0) {
    Serial.println("No temperature in current_weather");
    return;
  }

  tIdx += strlen("\"temperature\":");
  int tEnd = cw.indexOf(",", tIdx);
  String temp = cw.substring(tIdx, tEnd);

  Serial.print("Temperature in Da Nang: ");
  Serial.print(temp);
  Serial.println(" C");
}





















