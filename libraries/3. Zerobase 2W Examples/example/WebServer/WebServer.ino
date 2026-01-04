/*
  WebServer.ino - Example code to create a simple web server using WiFiEspAT library and control on-board LED
  This code connects to a WiFi network and starts a web server that allows users to turn the on-board LED on or off via a web interface.
  This also demonstrates the use of mDNS to access the server using a domain name. User accesses http://zerobase.local to control the LED.
  Bootstrap 5 is used for a modern and responsive web interface.
*/
#include <ZBPrint.h>

#define WIFIESPAT2
#include <WiFiEspAT.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiServer server(80);
bool ledState = false;

void setup() {
  Serial.begin(9600);
  delay(2000);

  pinMode(LED_BUILTIN, OUTPUT);  
  digitalWrite(LED_BUILTIN, LOW);

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
    WiFi.startMDNS("zerobase", "Zerobase 2W on-board LED control", 80);
  } else {
    Serial.println("Connect to WiFi failed");
  }

  printWifiStatus();
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  WiFiClient client = server.available();
  if(!client) return;

  String req = "";
  unsigned long tStart = millis();
  while(client.connected() && millis() - tStart < 500) {
    if(client.available()) {
      char c = client.read();  
      req += c;
    }
  }

  if(req.indexOf("favicon") >= 0) {
    client.flush();
    delay(10);
    client.stop();
    return;
  }

  if (req.indexOf("GET /?led=on") >= 0) {
    ledState = true;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (req.indexOf("GET /?led=off") >= 0) {
    ledState = false;
    digitalWrite(LED_BUILTIN, LOW);
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html lang='en'>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet'>");
  client.println("<title>Zerobase 2W on-board LED control</title>");
  client.println("</head>");
  client.println("<body class='container text-center mt-5'>");

  client.println("<div class='mb-4'>");
  client.println("<img src='https://cdn.chipstack.vn/chipstack/chipstack-logo-2.svg' alt='ChipStack Logo' style='height:80px;'>");
  client.println("</div>");

  client.println("<h1>Zerobase 2W on-board LED control</h1>");
  client.println("<p>LED is currently: <strong>" + String(ledState ? "ON" : "OFF") + "</strong></p>");

  client.println("<a href='/?led=on' class='btn btn-success btn-lg m-2'>Turn ON</a>");
  client.println("<a href='/?led=off' class='btn btn-danger btn-lg m-2'>Turn OFF</a>");

  client.println("</body></html>");

  client.flush();
  delay(50);
  client.stop();
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
















