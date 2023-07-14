#include "WiFi.h"

void setup() {
  Serial.begin(500000);
}

void loop() {
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print("db)");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "?" : "*");
      delay(10);
    }
  }
  Serial.println("");
  delay(5000);
}
