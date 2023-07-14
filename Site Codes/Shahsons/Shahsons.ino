#include <SPI.h>
#include <SD.h>

File dataFile;

/*============================================================================================================================================================================*/

// JSON Packet Serial Number 
int pack_num = 0;

// JSON Packet Storage Counter
int pack_count = 0;

// JSON Packet Storage Limit
int pack_limit = 50;

/*============================================================================================================================================================================*/

// Timer Increment Value
unsigned long timer_inc = 300000;

// JSON Packet Sending time Counter
unsigned long timer = 30000;

// Downtime Value i.e. 60 Seconds
unsigned long DT = 60000; 

/*============================================================================================================================================================================*/

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;

// Sensor 1, 2, 3, 4 Previous Status Bit
int PS1 = 0;
int PS2 = 0;

// Sensor`s Down Time Stamps for continuous low input 
unsigned long DTS1 = 0;
unsigned long DTS2 = 0;

// Sensor`s Continuous Time Stamps for continuous high input 
unsigned long CTS1 = 0;
unsigned long CTS2 = 0;

// Sensor`s Input Pin Values
int S1 = 0;
int S2 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;
int P2 = 0;

// Sensor`s Count Values 
unsigned long count1 = 0;
unsigned long count2 = 0;

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

  // Sensor 1 ( Good Output Count and Status ) ..............................................
  S1 = digitalRead(I0_12);
  if ( S1 == 1 && P1 == 0 ) { count1++; SS1=0; CTS1 = millis(); }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; }    
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }              
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }    
  P1 = S1;

  // Sensor 2 ( Good Output Count and Status ) ..............................................
  S2 = digitalRead(I0_11);
  if ( S2 == 1 && P2 == 0 ) { count2++; SS2=0; CTS2 = millis(); } 
  if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; }
  if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
  if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
  P2 = S2;

  // Event trigger on S1 .........................................................................
  if ( PS1 != SS1 )
  { 
    timer = millis()+timer_inc;
    write_to_sd(); 
    PS1 = SS1;
  }

  // Event trigger ON S2 .........................................................................
  if ( PS2 != SS2 )
  { 
    timer = millis()+timer_inc;
    write_to_sd();  
    PS2 = SS2;
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

  String pts = "\"PTS\":" + String(millis()) + "," ;
  String ptc = "\"PTC\":" + String(pack_num) + "," ;
  
  String sr1 = "\"SR1\":" + String(count1) + "," ;
  String sr2 = "\"SR2\":" + String(count2) + "," ;
  
  String ss1 = "\"SS1\":" + String(SS1) + "," ;
  String ss2 = "\"SS2\":" + String(SS2) ;

  String json = "{" + pts + ptc + sr1 + sr2 + ss1 + ss2 + "}" ;

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
