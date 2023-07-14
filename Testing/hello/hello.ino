#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

void setup()
{
  Serial.begin(500000);
  Serial.println("Initialize DS3231");;
  clock.begin();
  clock.setDateTime(__DATE__, __TIME__);
}

void loop()
{
  dt = clock.getDateTime();
  json();
  delay(1000);
}

void RTC_Date() { 
  Serial.print(dt.day);    Serial.print("/");
  Serial.print(dt.month);  Serial.print("/");
  Serial.print(dt.year);   Serial.println(",");
}

void RTC_Time() {
  Serial.print(dt.hour);   Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.println(",");
}

void json(){
  Serial.println("{");
  Serial.print("Date : ");  RTC_Date();
  Serial.print("Time : ");  RTC_Time();
  Serial.println("}");
}
