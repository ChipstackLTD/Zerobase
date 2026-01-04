/*
  NTP.ino - Example code to get current time from NTP server using WiFiEspAT library and WiFiUdp
  This code connects to a WiFi network and retrieves the current time from an NTP server.
  UDP (User Datagram Protocol) is used for NTP (Network Time Protocol) because:
  - Speed & Simplicity: UDP is connectionless and lightweight, so it allows quick transmission of small packets without the overhead of establishing and maintaining a connection (as TCP does).
  - Low Latency: NTP needs to send and receive small time requests/responses rapidly. UDP minimizes delay, which is important for accurate time synchronization.
  - Tolerance for Loss: NTP can tolerate occasional lost packets. If a response is missed, the client can simply retry. Reliable delivery (like TCP) isn’t necessary.
  - Broadcast/Multicast Support: UDP supports broadcasting and multicasting, which is useful for distributing time to many clients on a network.
  How it works in deep detail:
  - It sends a UDP packet to the NTP server requesting the current time.
  - The NTP server responds with a packet containing the current time in seconds since 1900.
  - The code extracts the time from the response, converts it to epoch time (seconds since 1970),
    and adjusts it to the local timezone (GMT+7 for Vietnam).
  - Finally, it prints the current time in HH:MM:SS format to the Serial monitor. 
*/
#include <ZBPrint.h>

#define WIFIESPAT2
#include <WiFiEspAT.h>
#include <WiFiUdp.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WiFiClient client;
WiFiUDP udp;
const char* ntpServer = "pool.ntp.org";
const int ntpPort = 123;

const int NTP_PACKET_SIZE = 48;
byte ntpBuffer[NTP_PACKET_SIZE];

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
  syncNTP();
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

void sendNTPpacket() {
  memset(ntpBuffer, 0, NTP_PACKET_SIZE);
  ntpBuffer[0] = 0b11100011; // LI, Version, Mode

  udp.beginPacket(ntpServer, ntpPort);
  udp.write(ntpBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void syncNTP() {
  udp.begin(2390);
  sendNTPpacket();

  delay(1500);

  if (udp.parsePacket()) {
    udp.read(ntpBuffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(ntpBuffer[40], ntpBuffer[41]);
    unsigned long lowWord  = word(ntpBuffer[42], ntpBuffer[43]);

    unsigned long secsSince1900 = (highWord << 16) | lowWord;
    const unsigned long seventyYears = 2208988800UL;

    unsigned long epoch = secsSince1900 - seventyYears;
    epoch += 7 * 3600; // VN GMT+7

    Serial.print("Epoch time: ");
    Serial.println(epoch);

    printTime(epoch);
  } else {
    Serial.println("NTP FAIL");
  }

  udp.stop();
}

void printTime(unsigned long epoch) {
  int second = epoch % 60;
  epoch /= 60;
  int minute = epoch % 60;
  epoch /= 60;
  int hour = epoch % 24;

  Serial.print("Gio VN: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
}























