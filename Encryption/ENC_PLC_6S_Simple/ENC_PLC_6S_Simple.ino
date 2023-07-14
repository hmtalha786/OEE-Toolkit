#include <ArduinoJson.h>

String json;

String Encrypted_Data;

//-------------------------------------------------------------------------------------------------------------------

char Ref_Array[90] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                      'z', 'y', 'x', 'w', 'v', 'u', 't', 's', 'r', 'q', 'p', 'o', 'n', 'm', 'l', 'k', 'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                      '{', '}', '[', ']', ' ', ',', '"', ':', '@', '?'
                     };

char Enc_Array[90] = {'(', ')', '_', ';', '$', ':', '-', '+', '*', '/',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                      'z', 'y', 'x', 'w', 'v', 'u', 't', 's', 'r', 'q', 'p', 'o', 'n', 'm', 'l', 'k', 'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a',
                      '9', '8', '7', '6', '5', '4', '3', '2', '1', '0'
                     };

//-------------------------------------------------------------------------------------------------------------------

// JSON Packet Sending time Counter
unsigned long timer = 15000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

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

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;
int SS3 = 0;
int SS4 = 0;
int SS5 = 0;
int SS6 = 0;

// Sensor`s Previous Bits ( 0 => ON , 1 => OFF )
int PS1 = 0;
int PS2 = 0;
int PS3 = 0;
int PS4 = 0;
int PS5 = 0;
int PS6 = 0;

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

// Sensor`s Count Values 
unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;
unsigned long count5 = 0;
unsigned long count6 = 0;

//-------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(500000);
}

//-------------------------------------------------------------------------------------------------------------------

void loop() {
  
  // Sensor 1 ..............................................................................
  S1 = digitalRead(I0_12);
  if ( S1 == 1 && P1 == 0 ) { CTS1 = millis(); DTS1 = 0; SS1=0; count1++; }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; SS1=0; }                      
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }         // Make Status 1 if sensor`s input remains low for 33s
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }         // Make Status 1 if sensor`s input remains high for 33s
  P1 = S1;

  // Sensor 2 ..............................................................................
  S2 = digitalRead(I0_11);
  if ( S2 == 1 && P2 == 0 ) { CTS2 = millis(); DTS2 = 0; SS2=0; count2++; } 
  if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; SS2=0; }
  if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
  if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
  P2 = S2;

  // Sensor 3 ..............................................................................
  S3 = digitalRead(I0_10);
  if ( S3 == 1 && P3 == 0 ) { CTS3 = millis(); DTS3 = 0; SS3=0; count3++; } 
  if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; SS3=0; }
  if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > DT ) ) { SS3=1; }
  if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > DT ) ) { SS3=1; }
  P3 = S3;

  // Sensor 4 ..............................................................................
  S4 = digitalRead(I0_9);
  if ( S4 == 1 && P4 == 0 ) { CTS4 = millis(); DTS4 = 0; SS4=0; count4++; } 
  if ( S4 == 0 && P4 == 1 ) { DTS4 = millis(); CTS4 = 0; SS4=0; }
  if ( S4 == 0 && P4 == 0 && ( ( millis() - DTS4 ) > DT ) ) { SS4=1; } 
  if ( S4 == 1 && P4 == 1 && ( ( millis() - CTS4 ) > DT ) ) { SS4=1; } 
  P4 = S4;

  // Sensor 5 ..............................................................................
  S5 = digitalRead(I0_8);
  if ( S5 == 1 && P5 == 0 ) { CTS5 = millis(); DTS5 = 0; SS5=0; count5++; } 
  if ( S5 == 0 && P5 == 1 ) { DTS5 = millis(); CTS5 = 0; SS5=0; }
  if ( S5 == 0 && P5 == 0 && ( ( millis() - DTS5 ) > DT ) ) { SS5=1; } 
  if ( S5 == 1 && P5 == 1 && ( ( millis() - CTS5 ) > DT ) ) { SS5=1; } 
  P5 = S5;

  // Sensor 6 ..............................................................................
  S6 = digitalRead(I0_7);
  if ( S6 == 1 && P6 == 0 ) { CTS6 = millis(); DTS6 = 0; SS6=0; count6++; } 
  if ( S6 == 0 && P6 == 1 ) { DTS6 = millis(); CTS6 = 0; SS6=0; }
  if ( S6 == 0 && P6 == 0 && ( ( millis() - DTS6 ) > DT ) ) { SS6=1; } 
  if ( S6 == 1 && P6 == 1 && ( ( millis() - CTS6 ) > DT ) ) { SS6=1; } 
  P6 = S6;

  // JSON Packet Sent after every 300 sec ...................................................          
  if ( millis() >= timer ) 
  { 
    timer = millis()+15000UL; 
    json_packet_sender(); 
  }

}

//-------------------------------------------------------------------------------------------------------------------

void json_packet_sender(){

  StaticJsonDocument<500> doc;
  
  doc["PTS"] = millis();
  
  doc["SR1"] = count1;
  doc["SR2"] = count2;
  doc["SR3"] = count3;
  doc["SR4"] = count4;
  doc["SR5"] = count5;
  doc["SR6"] = count6;
  
  doc["SS1"] = SS1;
  doc["SS2"] = SS2;
  doc["SS3"] = SS3;
  doc["SS4"] = SS4;
  doc["SS5"] = SS5;
  doc["SS6"] = SS6;
  
  serializeJson(doc, json);
  Encrypt(json);                 // Encrypt the json packet
  
  Serial.println(json);

  // Clear the variables
  json = "";
  Encrypted_Data = "";

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
}

//-------------------------------------------------------------------------------------------------------------------

void Encrypt(String Str) {

  int MVL = Str.length();                      // Message Value Length
  int RAL = sizeof(Ref_Array);                 // Refference Array Length

  for ( int i = 0; i < MVL; i++ ) {
    for ( int j = 0; j < RAL; j++ ) {
      if ( String(Str[i]) == String(Ref_Array[j]) ) {
        Encrypted_Data += String(Enc_Array[j]);
      }
    }
  }
  
  json = Encrypted_Data;

}
