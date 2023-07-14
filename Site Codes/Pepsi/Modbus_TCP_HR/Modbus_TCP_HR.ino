#include <Ethernet.h>
#include <ModbusTCPSlave.h>

// Ethernet configuration values
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
IPAddress ip(169, 254, 249, 150);
int port = 502;

// Number of holding register required
uint16_t Holding_Register[20];

// Define the ModbusTCPSlave object
ModbusTCPSlave modbus(port);

int i = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600UL);

  // Init Ethernet
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());

  // Init ModbusTCPSlave object
  modbus.begin();

  modbus.setHoldingRegisters(Holding_Register, 20);

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  Holding_Register[0] = 0330;    // Not giving right value
  Holding_Register[1] = 30000;   // 30,000 is the safe limit
  Holding_Register[2] = 2322;
  Holding_Register[3] = 2333;
  Holding_Register[4] = 4344;
  Holding_Register[5] = 2325;
  Holding_Register[6] = 6366;
  Holding_Register[7] = 7377;
  Holding_Register[8] = 8388;
  Holding_Register[9] = 9399;
  Holding_Register[10] = 1113;
  Holding_Register[11] = 1113;
  Holding_Register[12] = 2322;
  Holding_Register[13] = 3333;
  Holding_Register[14] = 3244;
  Holding_Register[15] = 5355;
  Holding_Register[16] = 6366;
  Holding_Register[17] = 7377;
  Holding_Register[18] = 8388;
  Holding_Register[19] = 9399;

  // Process modbus requests
  modbus.update();

//  i += 100;
//
//  delay(500);

}
