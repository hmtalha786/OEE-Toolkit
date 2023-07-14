#include <WiFi.h>
#include "time.h"
#include <RTC2.h>

const char* ssid = "Procheck";
const char* pass = "Procheck@123";

/*------------------------------------------------------------------------------------ Void Setup -------------------------------------------------------------------------------------*/

void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");
  connect_to_wifi();
  unsigned long epochTime = getTime();
  RTC.setTime(epochTime);
}

/*------------------------------------------------------------------------------------- Void Loop -------------------------------------------------------------------------------------*/

void loop() {
  Serial.print("Internet Timestamp: ");
  unsigned long rtc_timestamp = getTime();
  Serial.print("RTC Timestamp: ");
  Serial.println(rtc_timestamp);
  delay(1000);
}

/*--------------------------------------------------------------------------------- Get Time Function ----------------------------------------------------------------------------------*/

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time(&now);
  return now;
}

/*---------------------------------------------------------------------------------- Connect to WiFi ------------------------------------------------------------------------------------*/

void connect_to_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) { Serial.println("Connecting to WiFi ... "); delay(1000); }
  Serial.println(WiFi.localIP());
}
