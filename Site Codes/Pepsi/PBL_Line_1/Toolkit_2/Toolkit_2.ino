#include <Ethernet.h>
#include <ArduinoJson.h>
#include <ModbusTCPSlave.h>

///////////////////////////////////////////////////////////////////////////////////

// Ethernet configuration values
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

IPAddress ip(169, 254, 249, 150);

int port = 502;

///////////////////////////////////////////////////////////////////////////////////

// Number of holding register required
uint16_t Holding_Register[10];

// Define the ModbusTCPSlave object
ModbusTCPSlave modbus(port);

///////////////////////////////////////////////////////////////////////////////////

// JSON Packet Sending time Counter
unsigned long timer = 60000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

///////////////////////////////////////////////////////////////////////////////////

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;

// Sensor`s Input Pin Values
int S1 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;

// Sensor`s Count Values 
unsigned long count1 = 0;

// Sensor`s Down Time Stamps for continuous low input 
unsigned long DTS1 = 0;

// Sensor`s Continuous Time Stamps for continuous high input 
unsigned long CTS1 = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  
  // After Crate Unit Good Count & Status ......................................................
  S1 = digitalRead(I0_12);
  if ( S1 == 1 && P1 == 0 ) { CTS1 = millis(); DTS1 = 0; SS1=128; count1++; }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; SS1=128; }                      
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > 30000 ) ) { SS1=1; }
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > 30000 ) ) { SS1=1; }
  P1 = S1;

  // JSON Packet Sent after every 60 Seconds ...................................................          
  if ( millis() >= timer ) 
  { 
    timer = millis()+60000UL; 
    json_packet_sender(); 
  }

  // Stay Sync Modbus ..........................................................................
  modbus.update();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void json_packet_sender(){

  StaticJsonDocument<900> doc;
  
  doc["PTS"] = millis();
  doc["SR1"] = count1;
  doc["SS1"] = SS1;
  
  String json;
  serializeJson(doc, json);
  Serial.println(json);

  Holding_Register[1] = count1;   // Crate Unit Good Count
  Holding_Register[2] = SS1;      // Crate Unit Status ( 128 = Operating , 1 = Stopped )

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);

  // Reset Values
  count1 = 0;
  
}
