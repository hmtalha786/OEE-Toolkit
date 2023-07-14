#include <RS485.h>
#include <ModbusRTUMaster.h>

ModbusRTUMaster master(RS485);

unsigned long Counter = 15000;
unsigned long Cur_Count = 0;
unsigned long Pre_Count = 0;
unsigned long Speed = 0;

// Timer Increment Value
unsigned long timer_inc = 300000;

// JSON Packet Sending time Counter
unsigned long timer = 30000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;

// Sensor 1 Previous Status Bit
int PS1 = 0;

// Sensor`s Down Time Stamps for continuous low input
unsigned long DTS1 = 0;

// Sensor`s Continuous Time Stamps for continuous high input
unsigned long CTS1 = 0;

// Sensor`s Input Pin Values
int S1 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;

// Sensor`s Count Values
unsigned long count1 = 0;

void setup() {
  Serial.begin(9600UL);
  RS485.begin(9600UL, HALFDUPLEX, SERIAL_8E1);
}

void loop() {

  S1 = digitalRead(I0_12);
  
  if ( S1 == 1 && P1 == 0 ) {
    count1++;
    SS1 = 0;
    CTS1 = millis();
  }
  if ( S1 == 0 && P1 == 1 ) {
    DTS1 = millis();
    CTS1 = 0;
  }
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) {
    SS1 = 1;
  }
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) {
    SS1 = 1;
  }
  
  P1 = S1;

  // Speed Counter .........................................................................
  if ( millis() >= Counter )
  {
    Counter = millis() + 15000UL;
    Cur_Count = count1;

    Speed = ( Cur_Count - Pre_Count ) * 4 ;

    if ( Speed < 0 ) { Speed = 0; }

    Pre_Count = Cur_Count;

    uint16_t values[4];
    values[0] = (Speed / 1000 % 10) + 0x30;
    values[1] = (Speed / 100 % 10) + 0x30;
    values[2] = (Speed / 10 % 10) + 0x30;
    values[3] = (Speed % 10) + 0x30;

    if (master.writeMultipleRegisters(1, 0, values, 4)) {
      Serial.println(Speed);
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
}
