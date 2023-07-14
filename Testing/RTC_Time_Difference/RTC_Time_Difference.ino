#include <WiFi.h>
#include <time.h>
#include <RTC2.h>

const char* ssid = "Procheck";
const char* pass = "Procheck@123";

unsigned long unix_net_time;
unsigned long unix_rtc_time;
unsigned long unix_time_diff;

void setup() {
  Serial.begin(115200);
  connect_to_wifi();

  Serial.println("Configuring RTC via Internet Time : ");
  configTime(0, 0, "pool.ntp.org");
  
  unix_net_time = get_internet_time();
  unix_rtc_time = RTC.getTime();
  
  unix_time_diff = ( unix_rtc_time - unix_net_time ) + 162860160UL;

  Serial.print("Unix Internet Time : ");   Serial.println(unix_net_time);
  Serial.print("Unix RTC Timestamp : ");   Serial.println(unix_rtc_time);
  Serial.print("Unix Difference Time : "); Serial.println(unix_time_diff);

  // RTC.write writes in the RTC memory all that has been set
  if (!RTC.write()) {
    Serial.println("RTC Write Error: Are the switches well placed ???");
  }
}

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
    Serial.print(")");
    Serial.print(" Correct Timestamp : ");
    Serial.print(" (");
    Serial.print(RTC.getTime() - unix_time_diff);
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
unsigned long get_internet_time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    ESP.restart();
  }
//  Serial.print("Internet Timestamp: ");
//  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time(&now);
  return now;
}
