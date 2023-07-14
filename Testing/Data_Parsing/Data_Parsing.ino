#include <ArduinoJson.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(500000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {

    /* Create an Array as a buffer to store incoming serial data */
    char bfr[5001];
    memset(bfr, 0, 5001);

    /* readBytesUntil(Terminator, data, dataLength) */
    Serial.readBytesUntil('A', bfr, 5000);
    Serial.println(bfr);
    // Allocate Stack Memory for 1000 bytes
    StaticJsonDocument<1000> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, bfr);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // JSON Data Parsing
    const char* stp = doc["timestamp"];
    const char* val = doc["values"];

    // Print values
    Serial.println(stp);
    Serial.println(val);
  }
}
