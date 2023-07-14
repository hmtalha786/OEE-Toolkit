#include <ArduinoJson.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  if (SerialBT.available()) {

    char bfr[501];
    memset(bfr, 0, 501);

    /* readBytesUntil(Terminator, data, dataLength) */
    SerialBT.readBytesUntil('A', bfr, 500);
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
    int id = doc["id"];

    // Print values
    Serial.println(id);
  }

  delay(100);
}
