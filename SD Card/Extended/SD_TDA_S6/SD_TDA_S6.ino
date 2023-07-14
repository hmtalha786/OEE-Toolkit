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
int pack_limit = 30;

/*============================================================================================================================================================================*/

// Timer Increment Value
unsigned long timer_inc = 300000;

// JSON Packet Sending time Counter
unsigned long timer = 30000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000; 

/*============================================================================================================================================================================*/

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
    delay(100);
    SD.remove("data.txt");
    delay(100);
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
          delay(100);
          dataFile = SD.open("data.txt");
          delay(100);
          if (dataFile) { 
            Serial.print("{\"values\":[");
            for(int n = 0 ; n < (dataFile.size())-1 ; n++){ Serial.print(char(dataFile.read())); }
            Serial.println("]}");
            delay(100);
            dataFile.close();
            delay(100);
          }
        }
        break;
      // Indexing back to zero when Rx =4
      case 52:
        delay(100);
        SD.remove("data.txt");
        delay(100);
        pack_count = 0;
        timer_inc = 300000;     // 5 minutes
        delay(100);
        break;
    }
  }

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

  // Event trigger instantly if there is a change ..........................................
  if ( PS1 != SS1 )
  { 
    timer = millis()+timer_inc;
    write_to_sd(); 
    PS1 = SS1;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS3 != SS3 )
  { 
    timer = millis()+timer_inc;
    write_to_sd();  
    PS3 = SS3;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS5 != SS5 )
  { 
    timer = millis()+timer_inc;
    write_to_sd(); 
    PS5 = SS5;
  }

  // JSON Packet Sent after every 5 min ....................................................
  if ( millis() >= timer )
  {                 
     timer = millis()+timer_inc; 
     write_to_sd(); 
  }
  
}

/*============================================================================================================================================================================*/

void write_to_sd(){

  pack_count++;
  pack_num++;

  StaticJsonDocument<900> JSON_Entry;

  JSON_Entry["PTS"] = millis();
  JSON_Entry["PTC"] = pack_num;
  
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
​
  String json;
  serializeJson(JSON_Entry, json);

  /* Set the Time Driven Architecture to 15 minutes */
  if ( pack_count > 1 ) { timer_inc = 900000; }

  /* Clear the file if it reaches maximum data capacity */
  if ( pack_count > pack_limit ){ delay(100); SD.remove("data.txt"); delay(100); pack_count = 0; }

  /* Write to SD Card data File */
  dataFile = SD.open("data.txt", FILE_WRITE);
  delay(100);
  if (dataFile) { dataFile.print(json); dataFile.print(","); delay(100); dataFile.close(); delay(100); }
  delay(100);
  
  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
}
