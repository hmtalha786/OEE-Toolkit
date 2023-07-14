#include <Ethernet.h>
#include <ModbusTCPSlave.h>

int input_1;
int input_2;

float volt_1;
float volt_2;

float curr_1;
float curr_2;

float flow_1;
float flow_2;

String a_1;
String a_2;

String v_1;
String v_2;

String c_1;
String c_2;

String f_1;
String f_2;

String json_1;
String json_2;
    
// Ethernet configuration values
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
IPAddress ip(10, 23, 40, 10);
int port = 502;

// Modbus registers mapping
// This example uses the M-Duino21+ mapping
int digitalOutputsPins[] = {
#if defined(PIN_Q0_4)
  Q0_0, Q0_1, Q0_2, Q0_3, Q0_4,
#endif
};
int digitalInputsPins[] = {
#if defined(PIN_I0_6)
  I0_0, I0_1, I0_2, I0_3, I0_4, I0_5, I0_6,
#endif
};
int analogOutputsPins[] = {
#if defined(PIN_A0_7)
  A0_5, A0_6, A0_7,
#endif
};
int analogInputsPins[] = {
#if defined(PIN_I0_12)
  I0_7, I0_8, I0_9, I0_10, I0_11, I0_12,
#endif
};

#define numDigitalOutputs int(sizeof(digitalOutputsPins) / sizeof(int))
#define numDigitalInputs int(sizeof(digitalInputsPins) / sizeof(int))
#define numAnalogOutputs int(sizeof(analogOutputsPins) / sizeof(int))
#define numAnalogInputs int(sizeof(analogInputsPins) / sizeof(int))

bool digitalOutputs[numDigitalOutputs];
bool digitalInputs[numDigitalInputs];
uint16_t analogOutputs[numAnalogOutputs];
uint16_t analogInputs[numAnalogInputs];

// Define the ModbusTCPSlave object
ModbusTCPSlave modbus(port);

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600UL);

  // Init variables, inputs and outputs
  for (int i = 0; i < numDigitalOutputs; ++i) {
    digitalOutputs[i] = false;
    digitalWrite(digitalOutputsPins[i], digitalOutputs[i]);
  }
  for (int i = 0; i < numDigitalInputs; ++i) {
    digitalInputs[i] = digitalRead(digitalInputsPins[i]);
  }
  for (int i = 0; i < numAnalogOutputs; ++i) {
    analogOutputs[i] = 0;
    analogWrite(analogOutputsPins[i], analogOutputs[i]);
  }
  for (int i = 0; i < numAnalogInputs; ++i) {
    analogInputs[i] = analogRead(analogInputsPins[i]);
  }

  // Init Ethernet
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());

  // Init ModbusTCPSlave object
  modbus.begin();

  modbus.setCoils(digitalOutputs, numDigitalOutputs);
  modbus.setDiscreteInputs(digitalInputs, numDigitalInputs);
  modbus.setHoldingRegisters(analogOutputs, numAnalogOutputs);
  modbus.setInputRegisters(analogInputs, numAnalogInputs);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  //---------------------------------------------------------------------------------------------

  input_1 = analogRead(I0_12);
  
  volt_1 = map(input_1, 0, 1023, 0, 10000);
  curr_1 = map(input_1, 0, 1023, 0, 20000);
  flow_1 = map(input_1, 0, 1023, 0, 1300);

  a_1 = "\"IO_12 Analog Resolution\":" + String(input_1) + " bits" + "," ;
  v_1 = "\"Voltages\":" + String(volt_1/1000) + " volts" + "," ;
  c_1 = "\"Current\":" + String(curr_1/1000) + " mA" + "," ;
  f_1 = "\"Flow Value\":" + String(flow_1) + " kg/h" ;

  json_1 = "{" + a_1 + v_1 + c_1 + f_1 + "}" ;
  Serial.print(json_1);
  
  //---------------------------------------------------------------------------------------------

  input_2 = analogRead(I0_11);
  
  volt_2 = map(input_2, 0, 1023, 0, 10000);
  curr_2 = map(input_2, 0, 1023, 0, 20000);
  flow_2 = map(input_2, 0, 1023, 0, 1300);

  a_2 = "\"IO_11 Analog Resolution\":" + String(input_2) + " bits" + "," ;
  v_2 = "\"Voltages\":" + String(volt_2/1000) + " volts" + "," ;
  c_2 = "\"Current\":" + String(curr_2/1000) + " mA" + "," ;
  f_2 = "\"Flow Value\":" + String(flow_2) + " kg/h" ;

  json_2 = "{" + a_2 + v_2 + c_2 + f_2 + "}" ;
  Serial.println(json_2);

  //---------------------------------------------------------------------------------------------

  // Update inputs
  for (int i = 0; i < numDigitalInputs; ++i) {
    digitalInputs[i] = digitalRead(digitalInputsPins[i]);
  }
  for (int i = 0; i < numAnalogInputs; ++i) {
    analogInputs[i] = analogRead(analogInputsPins[i]);
  }
  
  // Process modbus requests
  modbus.update();

  // Update outputs
  for (int i = 0; i < numDigitalOutputs; ++i) {
    digitalWrite(digitalOutputsPins[i], digitalOutputs[i]);
  }
  for (int i = 0; i < numAnalogOutputs; ++i) {
    analogWrite(analogOutputsPins[i], analogOutputs[i]);
  }
}
