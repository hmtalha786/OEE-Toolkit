#include <Ethernet.h>
#include <ModbusTCPSlave.h>

// Ethernet configuration values
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
IPAddress ip(169, 254, 249, 150);
int port = 502;

// Number of holding register required
uint16_t Holding_Register[10];

// Define the ModbusTCPSlave object
ModbusTCPSlave modbus(port);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------

// Analog Input
uint16_t Input_1;
uint16_t Input_2;

// Voltage in Volts
float V1;
float V2;   

// Current in mA
float C1;
float C2;   

// Flow Value in Kg/h
float F1;
float F2;   

// Flow in gram/sec
float F1_GPS = 0;
float F2_GPS = 0;

// Totalizer of flow
uint16_t TF1 = 0;
uint16_t TF2 = 0;

// Total flow temporary
uint16_t TFT1 = 0;
uint16_t TFT2 = 0;

// Max Flow of meter
float Max_Flow = 1300;

//correction factor initilization
float CF = 1.03;

// Incremental index
uint16_t n = 0;

//---------------------------------------------------------------------------------------

void setup() {

  Serial.begin(9600UL);

  // Init Ethernet
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());

  // Init ModbusTCPSlave object
  modbus.begin();

  // Set Holding Register
  modbus.setHoldingRegisters(Holding_Register, 10);

}

//---------------------------------------------------------------------------------------

void loop() {

  // Read Analog Input
  Input_1 = analogRead(I0_12);
  Input_2 = analogRead(I0_11);

  // Scale Analog value to flow scale
  F1 = map( Input_1, 0, 1023, 0, Max_Flow );
  F2 = map( Input_2, 0, 1023, 0, Max_Flow );

  // Convert kg/h into g/s
  F1_GPS = ( F1 / 3.6 ) * CF;
  F2_GPS = ( F2 / 3.6 ) * CF;

  // Totalizer
  TFT1 = F1_GPS + TFT1;
  TFT2 = F2_GPS + TFT2;

  // Delay of 1 Second
  delay(1000);

  // Incremet Index
  n++;

  // Reset clock after 60 seconds
  if ( n >= 60 ) {

    TF1 = TFT1;
    TF2 = TFT2;

    Serial.print("Total Flow 1 : ");    Serial.println(TF1);
    Serial.print("Total Flow 2 : ");    Serial.println(TF2);

    // update data register value
    Holding_Register[1] = TF1;
    Holding_Register[2] = TF2;

    // Reset Values
    TFT1 = 0;
    TFT2 = 0;
    n = 0;

  }

  // Stay Sync Modbus
  modbus.update();

}
