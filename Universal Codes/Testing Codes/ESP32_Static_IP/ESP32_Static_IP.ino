#include <WiFi.h>
#include <ESP32Ping.h>

const char* ssid = "Procheck";
const char* pass = "Procheck@123";

IPAddress staticIP(192, 168, 18, 127);
IPAddress subnet(255, 255, 255, 0);
IPAddress gateway(192, 168, 18, 1);
IPAddress dns(192, 168, 18, 1);

/* =============================================================================================================================================================================== */

void setup() {
  Serial.begin(500000);
  Connect_To_WiFi(ssid, pass);
}

void loop() {
  Serial.print("RRSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println("db");
  delay(500);
}

void Connect_To_WiFi(const char * ssid, const char * pwd) {
  
  Serial.println("Connecting to WiFi network: " + String(ssid));
  
  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) { Serial.println("Configuration failed."); }
  
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Primary DNS: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("Secondary DNS: ");
  Serial.println(WiFi.dnsIP(1));

  bool success = Ping.ping("www.google.com", 3);
  
  if (!success) { Serial.println("Ping failed"); ESP.restart(); } else { Serial.println("Ping successful."); Serial.println(); }
}
