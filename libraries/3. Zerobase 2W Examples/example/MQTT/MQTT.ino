/*
  MQTT.ino - Example code to connect to MQTT broker using WiFiEspAT library and PubSubClient
  This code connects to a WiFi network and an MQTT broker, subscribes to a topic to receive messages,
  and publishes a message every 10 seconds.
*/
#include <ZBPrint.h>

#define WIFIESPAT2
#include <WiFiEspAT.h>
#include <PubSubClient.h>

#define WIFIESPAT_LOG_LEVEL LOG_LEVEL_DEBUG
#define LOG_OUTPUT Serial

/* ===== WIFI ===== */
const char* ssid     = "Quang Hung";
const char* password = "quanghung68";

/* ===== MQTT ===== */
const char* mqttServer = "freemqtt.chipstack.vn";
const int   mqttPort   = 1883;
const char* mqttTopic  = "/topic/mytopic01";

/* ===== OBJECT ===== */
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastSend = 0;

/* ===== SETUP ===== */
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

  /* MQTT CONFIG */
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setSocketTimeout(15);
  mqttClient.setKeepAlive(30);
  mqttClient.setCallback(mqttCallback);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/* ===== MQTT RECONNECT ===== */
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting MQTT...");
    if (mqttClient.connect("")) {
      Serial.println("OK");
      
      // Subscribe topic sau khi connect
      if (mqttClient.subscribe("/topic/mytopic2")) {
        Serial.println("Subscribed to /topic/mytopic2");
      } else {
        Serial.println("Subscribe failed");
      }
    } else {
      Serial.print("Fail, rc=");
      Serial.println(mqttClient.state());
      delay(3000);
    }
  }
}

/* ===== LOOP ===== */
void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  if (millis() - lastSend >= 10000) {
    lastSend = millis();
    mqttClient.publish(mqttTopic, "hello");
    Serial.println("Sent message hello to /topic/mytopic1");
  }
}

/* ===== WIFI STATUS ===== */
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

  WiFiClient test;
  Serial.print("Test TCP to broker...");
  if (test.connect(mqttServer, mqttPort)) {
    Serial.println("OK");
    test.stop();
  } else {
    Serial.println("FAIL");
  }
}
