#include <ArduinoJson.h>

#define INTERRUPT_1 I0_5
#define INTERRUPT_2 I0_6

// JSON Packet Sending time Counter
unsigned long timer = 30000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;

// Sensor 1 Previous Status Bit
int PS1 = 0;

// Sensor`s Input Pin Values
volatile int S1 = 0;
volatile int S2 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;
int P2 = 0;

// Sensor`s Count Values
unsigned long count1 = 0;
unsigned long count2 = 0;

// Sensor`s Down Time Stamps for continuous FALLING input
unsigned long DTS1 = 0;
unsigned long DTS2 = 0;

// Sensor`s Continuous Time Stamps for continuous RISING input
unsigned long CTS1 = 0;
unsigned long CTS2 = 0;


unsigned long lastmillis = 0;

// ==========================================================================================================================================================

void setup() {
  Serial.begin(500000);

  pinMode(INTERRUPT_1, INPUT_PULLUP);
  pinMode(INTERRUPT_2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_1), CB_function_RISING_1, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_1), CB_function_FALLING_1, FALLING);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_2), CB_function_RISING_2, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_2), CB_function_FALLING_2, FALLING);
}

// ==========================================================================================================================================================

void CB_function_RISING_1() {
  S1 = 1;
}

void CB_function_FALLING_1() {
  S1 = 0;
}

void CB_function_RISING_2() {
  S2 = 1;
}

void CB_function_FALLING_2() {
  S2 = 0;
}

// ==========================================================================================================================================================

void loop() {

  if ( ( millis() - lastmillis ) > 50UL ) {

  detachInterrupt(digitalPinToInterrupt(INTERRUPT_1));
  detachInterrupt(digitalPinToInterrupt(INTERRUPT_2));
  
  // Sensor 1 ...............................................................................
  if ( S1 == 1 && P1 == 0 ) { count1++; SS1=0; CTS1 = millis(); }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; }                      
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }         // Make Status 1 if sensor`s input remains FALLING for 33s
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }         // Make Status 1 if sensor`s input remains RISING for 33s
  P1 = S1;

  // Sensor 2 ...............................................................................
  if ( S2 == 1 && P2 == 0 ) { count2++; SS2=0; CTS2 = millis(); } 
  if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; }
  if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
  if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
  P2 = S2;

  // Event trigger for SS1 ( i.e. 0 -> 1 or 1 -> 0 ) ........................................
  if( PS1 != SS1 ){ 
    timer = millis()+60000UL; 
    json_packet_sender(); 
    PS1 = SS1;  
  }

  // JSON Packet Sent after every 300 sec ...................................................          
  if ( millis() >= timer ) { 
    timer = millis()+60000UL; 
    json_packet_sender(); 
  }

  lastmillis = millis();

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_1), CB_function_RISING_1, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_1), CB_function_FALLING_1, FALLING);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_2), CB_function_RISING_2, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_2), CB_function_FALLING_2, FALLING);

  }
}

// ==========================================================================================================================================================

void json_packet_sender() {

  StaticJsonBuffer<300> JSON_Packet;
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["PTS"] = millis();
  JSON_Entry["SR1"] = count1;
  JSON_Entry["SR2"] = count2;
  JSON_Entry["SS1"] = SS1;
  JSON_Entry["SS2"] = SS2;

  char JSONmessageBuffer[300];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  Serial.print(JSONmessageBuffer);
  Serial.println('A');

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
}
