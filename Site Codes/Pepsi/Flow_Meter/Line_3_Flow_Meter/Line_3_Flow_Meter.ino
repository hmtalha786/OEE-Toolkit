#include <Ethernet.h>
#include <ModbusTCPSlave.h>

//---------------------------------------------------------------------------------------

// Ethernet configuration values
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

IPAddress ip(169, 254, 249, 150);

int port = 502;

//---------------------------------------------------------------------------------------

uint16_t Holding_Register[10];      // Number of holding register required

ModbusTCPSlave modbus(port);        // Define the ModbusTCPSlave object

//---------------------------------------------------------------------------------------

float Input_1;    // Analog Input

float V1;         // Voltage in Volts

float C1;         // Current in mA

float F1;         // Flow Value in Kg/h

//---------------------------------------------------------------------------------------

float Max_Flow = 1300;     // Max Flow of meter

float F1_GPS = 0;          // Flow in gram/s

float TF1 = 0;             // Totalizer of flow

float Acc_1 = 0;           // Accumulator Variable

//---------------------------------------------------------------------------------------

float CF = 1.07;    // Correction Factor

int n = 0;          // Main Loop Incremental index

int i = 0;          // Internal Loop Accumulator Index

//---------------------------------------------------------------------------------------

void setup() {

  Serial.begin(9600UL);

  // Init Ethernet
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());

  // Init ModbusTCPSlave object and set holding register
  modbus.begin();
  modbus.setHoldingRegisters(Holding_Register, 10);

}

//---------------------------------------------------------------------------------------

void loop() {

  ///////////////////////////////////////////////////////////////////////////////////////

  // Accumulate data every 100ms for 1 second ( 10 iterations )
  while ( i < 10 ) {

    // Read Analog Input
    Input_1 = analogRead(I0_12);

    // Scale Analog value to flow scale
    F1 = map( Input_1, 0, 1023, 0, Max_Flow );

    // Accumulator for Flow Value
    Acc_1 += F1;

    // Increment Index after every 100ms
    delay(100);
    i++;

    // Stay Sync Modbus
    modbus.update();

  }

  //////////////////////////////////////////////////////////////////////////////////////

  // Convert kg/h into g/s
  F1_GPS = ( ( Acc_1 * 0.1 ) / 3.6 ) * CF;

  // Totalizer of flow in grams
  TF1 += F1_GPS;

  //////////////////////////////////////////////////////////////////////////////////////

  // Incremet Index after every 1 sec
  Acc_1 = 0;
  i = 0;
  n++;

  //////////////////////////////////////////////////////////////////////////////////////

  // Reset clock after 1 minute and update data register
  if ( n >= 60 ) {

    Serial.println("Total Flow 1 : " + String(TF1));

    // update data register value
    Holding_Register[1] = TF1;

    // Reset Values
    TF1 = 0;
    n = 0;

  }

  //////////////////////////////////////////////////////////////////////////////////////

  // Stay Sync Modbus
  modbus.update();

}
