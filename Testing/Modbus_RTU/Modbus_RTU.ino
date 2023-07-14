#include <RS485.h>
#include <ModbusRTUMaster.h>

ModbusRTUMaster master(RS485);

int i = 0;

uint32_t lastSentTime = 0UL;
const uint32_t baudrate = 9600UL;

////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600UL);
  RS485.begin(baudrate, HALFDUPLEX, SERIAL_8E1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  uint16_t values[4];

  values[0] = (i / 1000 % 10) + 0x30;
  values[1] = (i / 100 % 10) + 0x30;
  values[2] = (i / 10 % 10) + 0x30;
  values[3] = (i % 10) + 0x30;

  if (master.writeMultipleRegisters(1, 0, values, 4)) {
    i++;
    delay(500);
  }

  if (master.isWaitingResponse()) {
    ModbusResponse response = master.available();
    if (response) {
      if (response.hasError()) {
        Serial.print("Error ");
        Serial.println(response.getErrorCode());
      } else {
        if (response.hasError()) {
          Serial.print("Error ");
          Serial.println(response.getErrorCode());
        } else {
          Serial.println("Done");
        }
      }
    }
  }
}
