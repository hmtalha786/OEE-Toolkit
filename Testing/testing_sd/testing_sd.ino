#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>

File dataFile;

File One;
File Two;
File Three;
File Four;
File Five;

// JSON Packet Counter
int Packet_Count = 0;

// Disconnect Packet Counter
int Dis_Packet_Count = 0;

// JSON Packet Sending time Counter
unsigned long timer = 30000; // 30 second

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;
int SS3 = 0;
int SS4 = 0;
int SS5 = 0;
int SS6 = 0;

// Sensor 1, 3, 5 Previous Status Bit
int PS1 = 0;
int PS3 = 0;
int PS5 = 0;

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

/*============================================================================================================================================================================*/

void setup() {
  Serial.begin(500000);  
  
  if (SD.begin(53)) { 
    Serial.println("SD Card Connected");
    digitalWrite(Q0_0, HIGH); 
    digitalWrite(Q0_1, HIGH); 
  } else { 
    Serial.println("SD Card not Connected");
    digitalWrite(Q0_0, LOW); 
    digitalWrite(Q0_1, LOW); 
  }
}

/*============================================================================================================================================================================*/

void loop() {

  // Read serial feedback from wifi shield .................................................
  if ( Serial.available() > 0 ) {
    switch (Serial.read()) {
      // Looping over an Array when Rx = 3
      case 51:
        if ( 0 < Dis_Packet_Count < 31 ) { send_first_file(); }
        if ( 30 < Dis_Packet_Count < 61 ) { send_second_file(); }
        if ( 60 < Dis_Packet_Count < 91 ) { send_third_file(); }
        if ( 90 < Dis_Packet_Count < 121 ) { send_fourth_file(); }
        if ( 120 < Dis_Packet_Count < 151 ) { send_fifth_file(); }
        break;
      // Indexing back to zero when Rx =4
      case 52:
        delay(500);
        SD.remove("data.txt");
        delay(500);
        Dis_Packet_Count += 1;
        delay(500);
        break;
    }
  }

  // Sensor 1 ..............................................................................
  S1 = digitalRead(I0_12);
  if ( S1 == 1 && P1 == 0 ) { count1++; SS1=0; CTS1 = millis(); }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; }    
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }              
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }    
  P1 = S1;

  // Sensor 2 ..............................................................................
  S2 = digitalRead(I0_11);
  if ( S2 == 1 && P2 == 0 ) { count2++; SS2=0; CTS2 = millis(); } 
  if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; }
  if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
  if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
  P2 = S2;

  // Sensor 3 ..............................................................................
  S3 = digitalRead(I0_10);
  if ( S3 == 1 && P3 == 0 ) { count3++; SS3=0; CTS3 = millis(); } 
  if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; }
  if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > DT ) ) { SS3=1; }
  if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > DT ) ) { SS3=1; }
  P3 = S3;

  // Sensor 4 ..............................................................................
  S4 = digitalRead(I0_9);
  if ( S4 == 1 && P4 == 0 ) { count4++; SS4=0; CTS4 = millis(); } 
  if ( S4 == 0 && P4 == 1 ) { DTS4 = millis(); CTS4 = 0; }
  if ( S4 == 0 && P4 == 0 && ( ( millis() - DTS4 ) > DT ) ) { SS4=1; } 
  if ( S4 == 1 && P4 == 1 && ( ( millis() - CTS4 ) > DT ) ) { SS4=1; } 
  P4 = S4;

  // Sensor 5 ..............................................................................
  S5 = digitalRead(I0_8);
  if ( S5 == 1 && P5 == 0 ) { count5++; SS5=0; CTS5 = millis(); } 
  if ( S5 == 0 && P5 == 1 ) { DTS5 = millis(); CTS5 = 0; }
  if ( S5 == 0 && P5 == 0 && ( ( millis() - DTS5 ) > DT ) ) { SS5=1; } 
  if ( S5 == 1 && P5 == 1 && ( ( millis() - CTS5 ) > DT ) ) { SS5=1; } 
  P5 = S5;

  // Sensor 6 ..............................................................................
  S6 = digitalRead(I0_7);
  if ( S6 == 1 && P6 == 0 ) { count6++; SS6=0; CTS6 = millis(); } 
  if ( S6 == 0 && P6 == 1 ) { DTS6 = millis(); CTS6 = 0; }
  if ( S6 == 0 && P6 == 0 && ( ( millis() - DTS6 ) > DT ) ) { SS6=1; } 
  if ( S6 == 1 && P6 == 1 && ( ( millis() - CTS6 ) > DT ) ) { SS6=1; } 
  P6 = S6;

  // Event trigger instantly if there is a change ..........................................
  if ( PS1 != SS1 )
  { 
    timer = millis()+300000UL;
    write_to_sd(); 
    PS1 = SS1;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS3 != SS3 )
  { 
    timer = millis()+300000UL;
    write_to_sd();  
    PS3 = SS3;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS5 != SS5 )
  { 
    timer = millis()+300000UL;
    write_to_sd(); 
    PS5 = SS5;
  }

  // JSON Packet Sent after every 5 min ....................................................
  if ( millis() >= timer )
  {                 
     timer = millis()+300000UL; 
     write_to_sd(); 
  }
}

/*============================================================================================================================================================================*/

void write_to_sd(){

  Packet_Count += 1;               // Normal Packet Count 

  Dis_Packet_Count += 1;           // Disconnected Packet Count
  
  StaticJsonBuffer<300> JSON_Packet;   
  JsonObject& JSON_Entry = JSON_Packet.createObject(); 

  JSON_Entry["PTS"] = millis();
  JSON_Entry["PTC"] = Packet_Count;
  JSON_Entry["SR1"] = count1;
  JSON_Entry["SR2"] = count2;
  JSON_Entry["SR3"] = count3;
  JSON_Entry["SR4"] = count4;
  JSON_Entry["SR5"] = count5;
  JSON_Entry["SR6"] = count6;
  JSON_Entry["SS1"] = SS1;
  JSON_Entry["SS2"] = SS2;
  JSON_Entry["SS3"] = SS3;
  JSON_Entry["SS4"] = SS4;
  JSON_Entry["SS5"] = SS5;
  JSON_Entry["SS6"] = SS6; 

  char JSONmessageBuffer[300];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  String json = JSONmessageBuffer;

  if ( 0 < Dis_Packet_Count < 31 ) {
    One = SD.open("One.txt", FILE_WRITE);
    delay(500);
    if (One) { One.print(json); One.print(","); delay(500); One.close(); delay(500); }
  }

  if ( 30 < Dis_Packet_Count < 61 ) {
    Two = SD.open("Two.txt", FILE_WRITE);
    delay(500);
    if (Two) { Two.print(json); Two.print(","); delay(500); Two.close(); delay(500); }  
  }

  if ( 60 < Dis_Packet_Count < 91 ) {
    Three = SD.open("Three.txt", FILE_WRITE);
    delay(500);
    if (Three) { Three.print(json); Three.print(","); delay(500); Three.close(); delay(500); }  
  }

  if ( 90 < Dis_Packet_Count < 121 ) {
    Four = SD.open("Four.txt", FILE_WRITE);
    delay(500);
    if (Four) { Four.print(json); Four.print(","); delay(500); Four.close(); delay(500); }
  }

  if ( 120 < Dis_Packet_Count < 151 ) {
    Five = SD.open("Five.txt", FILE_WRITE);
    delay(500);
    if (Five) { Five.print(json); Five.print(","); delay(500); Five.close(); delay(500); }  
  }
  
  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
}

// ==================================================================================================================================================================================

void send_first_file() {
  delay(500);
  One = SD.open("One.txt");
  delay(500);
  if (One) { 
    Serial.print("{\"values\":[");
    for(int n = 0 ; n < (One.size())-1 ; n++){ Serial.print(char(One.read())); }
    Serial.println("]}");
    delay(500);
    One.close();
    delay(500);
  } 
}

// ==================================================================================================================================================================================

void send_second_file() {
  delay(500);
  Two = SD.open("Two.txt");
  delay(500);
  if (Two) { 
    Serial.print("{\"values\":[");
    for(int n = 0 ; n < (Two.size())-1 ; n++){ Serial.print(char(Two.read())); }
    Serial.println("]}");
    delay(500);
    Two.close();
    delay(500);
  } 
}

// ==================================================================================================================================================================================

void send_third_file() {
  delay(500);
  Three = SD.open("Three.txt");
  delay(500);
  if (Three) { 
    Serial.print("{\"values\":[");
    for(int n = 0 ; n < (Three.size())-1 ; n++){ Serial.print(char(Three.read())); }
    Serial.println("]}");
    delay(500);
    Three.close();
    delay(500);
  }   
}

// ==================================================================================================================================================================================

void send_fourth_file() {
  delay(500);
  Four = SD.open("Four.txt");
  delay(500);
  if (Four) { 
    Serial.print("{\"values\":[");
    for(int n = 0 ; n < (Four.size())-1 ; n++){ Serial.print(char(Four.read())); }
    Serial.println("]}");
    delay(500);
    Four.close();
    delay(500);
  }  
}

// ==================================================================================================================================================================================

void send_fifth_file() {
  delay(500);
  Five = SD.open("Five.txt");
  delay(500);
  if (Five) { 
    Serial.print("{\"values\":[");
    for(int n = 0 ; n < (Five.size())-1 ; n++){ Serial.print(char(Five.read())); }
    Serial.println("]}");
    delay(500);
    Five.close();
    delay(500);
  }
}
