#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

File dataFile;

/*============================================================================================================================================================================*/

// JSON Packet Serial Number 
int pack_num = 0;

// JSON Packet Storage Counter
int pack_count = 0;

// JSON Packet Storage Limit
int pack_limit = 20;

/*============================================================================================================================================================================*/

// Timer Increment Value
unsigned long timer_inc = 300000;

// JSON Packet Sending time Counter
unsigned long timer = 30000;

// Speed Timer
unsigned long ST = 60000;

// Downtime Value i.e. 1 Minute or 60 Seconds
unsigned long DT = 60000;

// Line Speed in RPM
unsigned long Speed = 0;

// Current Status of the line
int CST = 0;

// Previous Status of the line
int PST = 0;

/*============================================================================================================================================================================*/

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;
int SS3 = 0;
int SS4 = 0;
int SS5 = 0;
int SS6 = 0;

// Sensor Previous Status Bit
int PS1 = 0;
int PS2 = 0;
int PS3 = 0;
int PS4 = 0;
int PS5 = 0;
int PS6 = 0;

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
    delay(10);
    SD.remove("data.txt");
    delay(10);
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
        if ( pack_count > 0 ) {
          if ( pack_count > 1 ) { write_to_sd(); }
          delay(10);
          dataFile = SD.open("data.txt");
          delay(10);
          if (dataFile) { 
            Serial.print("{\"values\":[");
            for(int n = 0 ; n < (dataFile.size())-1 ; n++){ Serial.print(char(dataFile.read())); }
            Serial.println("]}");
            delay(10);
            dataFile.close();
            delay(10);
          }
        }
        break;
      // Indexing back to zero when Rx =4
      case 52:
        delay(10);
        SD.remove("data.txt");
        delay(10);
        pack_count = 0;
        timer_inc = 300000;     // 5 minutes
        delay(10);
        break;
    }
  }

  // Sensor 1 ( Total Count ).................................................
  S1 = digitalRead(I0_12);
  if ( S1 == 1 && P1 == 0 ) { CTS1 = millis(); DTS1 = 0; SS1=0; count1++; }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; SS1=0; }                      
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }         // Make Status 1 if sensor`s input remains low for 33s
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }         // Make Status 1 if sensor`s input remains high for 33s
  P1 = S1;

  // Sensor 2 ( Good Count ) .................................................
  S2 = digitalRead(I0_11);
  if ( S2 == 1 && P2 == 0 ) { CTS2 = millis(); DTS2 = 0; SS2=0; count2++; } 
  if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; SS2=0; }
  if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
  if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
  P2 = S2;

  // Sensor 3 ( Speed in RPM ) ...............................................
  S3 = digitalRead(I0_10);
  if ( S3 == 1 && P3 == 0 ) { CTS3 = millis(); DTS3 = 0; SS3=0; count3++; } 
  if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; SS3=0; }
  if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > DT ) ) { SS3=1; }
  if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > DT ) ) { SS3=1; }
  P3 = S3;

  // Line status calculation .................................................
  if ( SS2 == 0 && SS3 == 0 ) { CST = 0; }    // Up
  if ( SS2 == 1 && SS3 == 1 ) { CST = 1; }    // Down
  if ( SS2 == 1 && SS3 == 0 ) { CST = -1; }   // Stop

  // Speed Counter ...........................................................
  if ( millis() >= ST ) 
  { 
    ST = millis() + 60000UL;  
    Speed = count3;  
    count3 = 0; 
  }
  
  // Event Triger on status change ...........................................
  if ( PST != CST )
  { 
    timer = millis() + timer_inc;  
    write_to_sd();
    PST = CST; 
  }

  // JSON Packet Sent after every 5 min .....................................
  if ( millis() >= timer )
  {                 
     timer = millis() + timer_inc; 
     write_to_sd(); 
  }
  
}

/*============================================================================================================================================================================*/

void write_to_sd(){

  pack_count++;
  pack_num++;

  StaticJsonDocument<500> doc;
  
  doc["PTS"] = millis();
  doc["PTC"] = pack_num;
  
  doc["SR1"] = count1;
  doc["SR2"] = count2;
  
  doc["RPM"] = Speed;
  doc["SS1"] = CST;
  
  String json;
  serializeJson(doc, json);

  /* Set the Time Driven Architecture to 15 minutes */
  if ( pack_count > 1 ) { timer_inc = 900000; }

  /* Clear the file if it reaches maximum data capacity */
  if ( pack_count > pack_limit ){ delay(10); SD.remove("data.txt"); delay(10); pack_count = 0; }

  /* Write to SD Card data File */
  dataFile = SD.open("data.txt", FILE_WRITE);
  delay(10);
  if (dataFile) { dataFile.print(json); dataFile.print(","); delay(10); dataFile.close(); delay(10); }
  delay(10);
  
  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
  
}
