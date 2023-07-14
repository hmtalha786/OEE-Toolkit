#include <WiFi.h>
#include "time.h"
#include <RTC2.h>

// Replace with your network credentials
const char* ssid = "Procheck";
const char* pass = "Procheck@123";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600 * 5;     // 5 Hours difference from global time
const int   daylightOffset_sec = 0;       // No need for GMT and UTC

void setup() {
  Serial.begin(115200);
  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  connect_to_wifi();
  unsigned long epochTime = getTime();
  RTC.setTime(epochTime);
}

void loop() {
  unsigned long epoch_unix_timestamp = getTime();
  Serial.print("Epoch Unix Timestamp: ");
  Serial.println(epoch_unix_timestamp);
  delay(1000);
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  Serial.print("Internet Time: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time(&now);
  return now;
}

// Initialize WiFi
void connect_to_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
