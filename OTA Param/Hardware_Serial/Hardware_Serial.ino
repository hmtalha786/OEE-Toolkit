#include <ArduinoJson.h>

char bfr[501];

////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////////////////////////

void loop() {

  if (Serial.available() > 0) {
    memset(bfr, 0, 501);
    Serial.readBytesUntil('~', bfr, 500);
    Serial.println(bfr);
    Parse_Data(bfr);
  }
  
  delay(100);
}

////////////////////////////////////////////////////////////////////////////////

void Parse_Data( String str) {

  StaticJsonDocument<500> doc;
  DeserializationError error = deserializeJson(doc, str);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // JSON Data Parsing
  String url = doc["URL"];      Serial.println(url);
  String sas = doc["SAS"];      Serial.println(sas);
  String acs = doc["ACS"];      Serial.println(acs);

  String wn1 = doc["WN1"];      Serial.println(wn1);
  String wp1 = doc["WP1"];      Serial.println(wp1);
  String wn2 = doc["WN2"];      Serial.println(wn2);
  String wp2 = doc["WP2"];      Serial.println(wp2);
  
  int nsr = doc["NSR"];      Serial.println(nsr);
  int esr = doc["ESR"];      Serial.println(esr);
  int mpl = doc["MPL"];      Serial.println(mpl);

}
