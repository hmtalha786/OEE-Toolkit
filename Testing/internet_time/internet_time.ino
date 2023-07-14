#include <WiFi.h>
#include "time.h"

const char* ssid = "Procheck Team";
const char* pass = "pc@54321";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 5;     // 5 Hours difference from global time
const int   daylightOffset_sec = 0;       // No need for GMT and UTC

void setup()
{
  Serial.begin(9600);
  connect_to_wifi();

  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  delay(1000);
  printLocalTime();
}

void connect_to_wifi() {
  // connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
