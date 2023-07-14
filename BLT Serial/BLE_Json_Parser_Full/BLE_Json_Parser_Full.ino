#include <ArduinoJson.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("Procheck_Test");       //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  if (SerialBT.available()) {

    char bfr[501];
    memset(bfr, 0, 501);

    /* readBytesUntil(Terminator, data, dataLength) */
    SerialBT.readBytesUntil('!', bfr, 500);
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
    String URL = doc["url"];     // Full HTTP Azure Endpoint
    String SAS = doc["sas"];     // Full SAS Token

    int NSR = doc["nsr"];        // Normal Scan Rate
    int ESR = doc["esr"];        // Extended Scan Rate
    int MPL = doc["mpl"];        // Max Packet Limit

    String WN1 = doc["wn1"];     // Wifi Network 1
    String WN2 = doc["wn2"];     // Wifi Network 2
    String WP1 = doc["wp1"];     // Wifi Password 1
    String WP2 = doc["wp2"];     // Wifi Password 2

    // Print values
    Serial.print("URL : ");   Serial.println(URL);
    Serial.print("SAS : ");   Serial.println(SAS);
    Serial.print("NSR : ");   Serial.println(NSR);
    Serial.print("ESR : ");   Serial.println(ESR);
    Serial.print("MPL : ");   Serial.println(MPL);
    Serial.print("WN1 : ");   Serial.println(WN1);
    Serial.print("WN2 : ");   Serial.println(WN2);
    Serial.print("WP1 : ");   Serial.println(WP1);
    Serial.print("WP2 : ");   Serial.println(WP2);

  }

  delay(100);

}
