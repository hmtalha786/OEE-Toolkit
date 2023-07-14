#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>

File dataFile;

/*============================================================================================================================================================================*/

// JSON Packet Serial Number 
int pack_num = 0;

// JSON Packet Storage Counter
int pack_count = 0;

// JSON Packet Storage Limit
int pack_limit = 60;

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

// Sensor 1 Previous Status Bit
int PS1 = 0;

// Sensor`s Down Time Stamps for continuous low input 
unsigned long DTS1 = 0;

// Sensor`s Continuous Time Stamps for continuous high input 
unsigned long CTS1 = 0;

// Sensor`s Input Pin Values
int S1 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;

// Sensor`s Count Values 
unsigned long count1 = 0;

// SD Card status whether it is working or not
bool SD_Status;

/*============================================================================================================================================================================*/

void setup() {
  
  Serial.begin(500000);  
  
  if (SD.begin(53)) { 
    Serial.println("SD Card Connected");
    digitalWrite(Q0_0, HIGH); 
    digitalWrite(Q0_1, HIGH); 
    SD_Status = true;
    delay(100);
    SD.remove("data.txt");
    delay(100);
  } else { 
    Serial.println("SD Card not Connected");
    digitalWrite(Q0_0, LOW); 
    digitalWrite(Q0_1, LOW); 
    SD_Status = false;
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

  // Event trigger instantly if there is a change ..........................................
  if ( PS1 != SS1 )
  { 
    timer = millis()+timer_inc;
    if( SD_Status ){ write_to_sd(); } else { create_json(); }
    PS1 = SS1;
  }

  // JSON Packet Sent after every 5 min ....................................................
  if ( millis() >= timer )
  {                 
     timer = millis()+timer_inc; 
     if( SD_Status ){ write_to_sd(); } else { create_json(); }
  }
  
}

/*============================================================================================================================================================================*/

void write_to_sd(){

  pack_count++;
  pack_num++;

  StaticJsonDocument<300> doc;
  
  doc["PTS"] = millis();
  doc["PTC"] = pack_num;
  doc["SR1"] = count1;
  doc["SS1"] = SS1;
  
  String json;
  serializeJson(doc, json);

  /* Write to SD Card data File */
  dataFile = SD.open("data.txt", FILE_WRITE);
  delay(100);
  if (dataFile) { dataFile.print(json); dataFile.print(","); delay(100); dataFile.close(); delay(100); }
  delay(100);

  /* Set the Time Driven Architecture to 15 minutes */
  if ( pack_count > 1 ) { timer_inc = 900000; }

  /* Clear the file if it reaches maximum data capacity */
  if ( pack_count > pack_limit ){ delay(100); SD.remove("data.txt"); delay(100); pack_count = 0; }

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
  
}

/*============================================================================================================================================================================*/

void create_json(){

  pack_num++;

  StaticJsonDocument<300> doc;
  
  doc["PTS"] = millis();
  doc["PTC"] = pack_num;
  doc["SR1"] = count1;
  doc["SS1"] = SS1;
  
  String json;
  serializeJson(doc, json);
  Serial.println(json);

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
  
}
