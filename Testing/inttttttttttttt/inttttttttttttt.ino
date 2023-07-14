#include <ArduinoJson.h>

#define INTERRUPT_1 I0_5
#define INTERRUPT_2 I0_6

unsigned long pack_timer = 60000;
unsigned long check_timer = 1000;

// Downt and Continous Time Checks
volatile unsigned long check_1 = 0;
volatile unsigned long check_2 = 0;

// Sensor`s Input Pin Values
volatile int SS1 = 0;
volatile int SS2 = 0;

// Sensor`s Count Values
volatile unsigned long count_1 = 0;
volatile unsigned long count_2 = 0;

void setup() {
  Serial.begin(500000);

  pinMode(INTERRUPT_1, INPUT_PULLUP);
  pinMode(INTERRUPT_2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_1), CB_function_RISING_1, RISING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_2), CB_function_RISING_2, RISING);

  Serial.println('Sensor started');
}

////////////////////////////////////////////////////////////

void loop() {
  
  // JSON Packet Sent after every 300 sec ...................................................
  if ( millis() >= pack_timer ) {
    detachInterrupt(digitalPinToInterrupt(INTERRUPT_1));
    detachInterrupt(digitalPinToInterrupt(INTERRUPT_2));

    pack_timer += millis();
    json_packet_sender();

    attachInterrupt(digitalPinToInterrupt(INTERRUPT_1), CB_function_RISING_1, RISING);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_2), CB_function_RISING_2, RISING);
  }
}

// ==========================================================================================================================================================

void CB_function_RISING_1() {
  count_1++;
  SS1 = 0;
}


void CB_function_RISING_2() {
  count_2++;
  SS2 = 0;
}

// ==========================================================================================================================================================

void json_packet_sender() {

  StaticJsonBuffer<300> JSON_Packet;
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["PTS"] = millis();
  JSON_Entry["SR1"] = count_1;
  JSON_Entry["SR2"] = count_2;
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
