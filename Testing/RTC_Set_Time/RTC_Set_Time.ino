#include <WiFi.h>
#include <time.h>
#include <RTC2.h>

const char* ssid = "Procheck";
const char* pass = "Procheck@123";

/*------------------------------------------------------------------------------------ Void Setup -------------------------------------------------------------------------------------*/

void setup() {
  Serial.begin(115200);
  connect_to_wifi();

  Serial.println("Configuring RTC via Internet Time : ");
  configTime(0, 0, "pool.ntp.org");
  unsigned long epochTime = getTime();
  RTC.setTime(epochTime);
  Serial.print(" (");
  Serial.print(epochTime);
  Serial.println(")");

  // RTC.write writes in the RTC memory all that has been set
  if (!RTC.write()) {
    Serial.println("RTC Write Error: Are the switches well placed ???");
  }
}

/*------------------------------------------------------------------------------------- Void Loop -------------------------------------------------------------------------------------*/

void loop() {
  if (!RTC.read()) {
    Serial.println("Read date error: is time set?");
  } else {
    Serial.print("RTC Unix Time: ");
    Serial.print(RTC.getYear());
    Serial.print("-");
    Serial.print(RTC.getMonth());
    Serial.print("-");
    Serial.print(RTC.getMonthDay());
    Serial.print(" ");
    Serial.print(RTC.getHour());
    Serial.print(":");
    Serial.print(RTC.getMinute());
    Serial.print(":");
    Serial.print(RTC.getSecond());
    Serial.print(" (");
    Serial.print(RTC.getTime());
    Serial.println(")");
  }

  delay(1000);
}

/*---------------------------------------------------------------------------------- Connect to WiFi ------------------------------------------------------------------------------------*/

void connect_to_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi ... ");
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

/*--------------------------------------------------------------------------------- Get Time Function ----------------------------------------------------------------------------------*/

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    ESP.restart();
  }
  Serial.print("Internet Timestamp: ");
  Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time(&now);
  return now;
}
