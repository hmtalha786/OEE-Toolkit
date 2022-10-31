#include<EEPROM.h>

void setup() {
  Serial.begin(500000);
  EEPROM.begin(500);
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  delay(500);
  Serial.println("EEPROM Clear Done!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
