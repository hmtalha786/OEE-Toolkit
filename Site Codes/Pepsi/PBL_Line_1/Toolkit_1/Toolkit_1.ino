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
int SS2 = 0;
int SS3 = 0;
int SS4 = 0;
int SS5 = 0;
int SS6 = 0;

// Sensor`s Input Pin Values
int S1 = 0;
int S2 = 0;
int S3 = 0;
int S4 = 0;
int S5 = 0;
int S6 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;
int P5 = 0;
int P6 = 0;

// Sensor`s Count Values 
unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;
unsigned long count5 = 0;
unsigned long count6 = 0;

// Sensor`s Down Time Stamps for continuous low input 
unsigned long DTS1 = 0;
unsigned long DTS2 = 0;
unsigned long DTS3 = 0;
unsigned long DTS4 = 0;
unsigned long DTS5 = 0;
unsigned long DTS6 = 0;

// Sensor`s Continuous Time Stamps for continuous high input 
unsigned long CTS1 = 0;
unsigned long CTS2 = 0;
unsigned long CTS3 = 0;
unsigned long CTS4 = 0;
unsigned long CTS5 = 0;
unsigned long CTS6 = 0;

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
  
  // Before EBI Sensor Status ///////////////////////////////////////////////////////////////////////////////////
  S1 = digitalRead(I0_12);
  if ( S1 == 1 && P1 == 0 ) { CTS1 = millis(); DTS1 = 0; SS1=9; count1++; }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; SS1=9; }                      
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > 60000 ) ) { SS1=9; }
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > 60000 ) ) { SS1=10; }
  P1 = S1;

  // EBI Total Count & Status ///////////////////////////////////////////////////////////////////////////////////
  S2 = digitalRead(I0_11);
  if ( S2 == 1 && P2 == 0 ) { CTS2 = millis(); DTS2 = 0; SS2=128; count2++; } 
  if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; SS2=128; }
  if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > 30000 ) ) { SS2=1; }
  if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > 30000 ) ) { SS2=1; }
  P2 = S2;

  // EBI Good Count & Status ///////////////////////////////////////////////////////////////////////////////////
  S3 = digitalRead(I0_10);
  if ( S3 == 1 && P3 == 0 ) { CTS3 = millis(); DTS3 = 0; SS3=128; count3++; } 
  if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; SS3=128; }
  if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > 30000 ) ) { SS3=1; }
  if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > 30000 ) ) { SS3=1; }
  P3 = S3;

  // Before Filer Sensor Status ///////////////////////////////////////////////////////////////////////////////
  S4 = digitalRead(I0_9);
  if ( S4 == 1 && P4 == 0 ) { CTS4 = millis(); DTS4 = 0; SS4=9; count4++; } 
  if ( S4 == 0 && P4 == 1 ) { DTS4 = millis(); CTS4 = 0; SS4=9; }
  if ( S4 == 0 && P4 == 0 && ( ( millis() - DTS4 ) > 60000 ) ) { SS4=9; } 
  if ( S4 == 1 && P4 == 1 && ( ( millis() - CTS4 ) > 60000 ) ) { SS4=10; } 
  P4 = S4;

  // Filer Good Count & Status ..............................................................................
  S5 = digitalRead(I0_8);
  if ( S5 == 1 && P5 == 0 ) { CTS5 = millis(); DTS5 = 0; SS5=128; count5++; } 
  if ( S5 == 0 && P5 == 1 ) { DTS5 = millis(); CTS5 = 0; SS5=128; }
  if ( S5 == 0 && P5 == 0 && ( ( millis() - DTS5 ) > 30000 ) ) { SS5=1; } 
  if ( S5 == 1 && P5 == 1 && ( ( millis() - CTS5 ) > 30000 ) ) { SS5=1; } 
  P5 = S5;

  // JSON Packet Sent after every 1 Minute ...................................................          
  if ( millis() >= timer ) 
  { 
    timer = millis()+60000UL; 
    json_packet_sender(); 
  }

  // Stay Sync Modbus
  modbus.update();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void json_packet_sender(){

  StaticJsonDocument<900> doc;
  
  doc["PTS"] = millis();
  
  doc["EBI Total Count"] = count2;
  doc["EBI Good Count"] = count3;
  doc["Filer Good Count"] = count5;
  
  doc["Before EBI Status"] = SS1;
  doc["EBI TC Status"] = SS2;
  doc["EBI GC Status"] = SS3;
  doc["Before Filer Status"] = SS4;
  doc["Filer GC Status"] = SS5;
  
  String json;
  serializeJson(doc, json);
  Serial.println(json);

  Holding_Register[1] = count2;   // EBI Total Count
  Holding_Register[2] = count3;   // EBI Good Count
  Holding_Register[3] = count5;   // Filer Good Count
  
  Holding_Register[4] = SS1;      // Before EBI Status       ( 9 = Starved , 10 = Blocked )
  Holding_Register[5] = SS2;      // EBI Total Count Status  ( 128 = Operating , 1 = Stopped )
  Holding_Register[6] = SS3;      // EBI Good Count Status   ( 128 = Operating , 1 = Stopped )
  Holding_Register[7] = SS4;      // Before Filer Status     ( 9 = Starved , 10 = Blocked )
  Holding_Register[8] = SS5;      // Filer Good Count Status ( 128 = Operating , 1 = Stopped )

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);

  // Reset Values
  count1 = count2 = count3 = count4 = count5 = 0;
  
}
