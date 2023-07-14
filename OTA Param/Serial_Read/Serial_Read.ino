#include <ArduinoJson.h>

////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
}

////////////////////////////////////////////////////////////////////////////////

void loop() {

  if (Serial.available() > 0) {
    char bfr[501];
    memset(bfr, 0, 501);
    Serial.readBytesUntil('~', bfr, 500);
    Serial.println(bfr);

//    StaticJsonDocument<500> doc;
//    DeserializationError error = deserializeJson(doc, bfr);
//
//    // Test if parsing succeeds.
//    if (error) {
//      Serial.print(F("deserializeJson() failed: "));
//      Serial.println(error.f_str());
//      return;
//    }
//
//    // JSON Data Parsing
//    int nsr = doc["NSR"];      Serial.println(nsr);
//    int esr = doc["ESR"];      Serial.println(esr);
//    int mpl = doc["MPL"];      Serial.println(mpl);
//    int psf = doc["PSF"];      Serial.println(psf);
//    int mef = doc["MEF"];      Serial.println(mef);
//    float hfv = doc["HFV"];    Serial.println(hfv);
  }

}
