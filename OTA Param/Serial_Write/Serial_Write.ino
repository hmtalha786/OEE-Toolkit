#include <ArduinoJson.h>

//////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

/////////////////////////////////////////////////////////////////////

void loop() {
  json_packet_sender();
  delay(10000);
}

/////////////////////////////////////////////////////////////////////

void json_packet_sender(){

  StaticJsonDocument<500> doc;
  
  doc["NSR"] = 5;              // Normal Scan Rate
  doc["ESR"] = 15;             // Extended Scan Rate
  doc["MPL"] = 30;             // Maximum Packet Limit
  doc["PSF"] = 1;              // Packet Sending Flag
  doc["MEF"] = 0;              // Memory Erase Flag
  doc["HFV"] = 3.6;            // Hardware Firmware Version
  
  String json;
  serializeJson(doc, json);
  Serial.print(json);
  Serial1.print(json);

}
