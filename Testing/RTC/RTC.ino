#include <RTC2.h>

//const int Time = 1664957180 + (3600 * 5);  // Additional 5 Hours for Pakistan Time

void setup() {
  Serial.begin(115200);
//  Serial.println("Configuring RTC: ");
//  Serial.println();
//  RTC.setTime(Time);

  // RTC.write writes in the RTC memory all that has been set
  if (!RTC.write()) {
    Serial.println("RTC Write Error: Are the switches well placed ???");
  }
}

void loop() {
  if (!RTC.read()) {
    Serial.println("Read date error: is time set?");
  } else {
    Serial.print("Time: ");
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
