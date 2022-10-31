#include <WiFi.h>

const char* ssid = "Procheck";
const char* pass = "Procheck@123";

void setup() {
  Serial.begin(500000);
  Connect_to_WiFi();
}

void loop() {
  Serial.print("RRSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println("db");
  delay(500);
}

void Connect_to_WiFi() {
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));
}
