#include <SPI.h>
#include <SD.h>

File data_file;

/*============================================================================================================================================================================*/

// Json Packet Carrier
String json;

// Number of SD_Card_File possible
char *SD_Card_File[] = {"d1.txt", "d2.txt", "d3.txt", "d4.txt", "d5.txt", "d6.txt", "d7.txt", "d8.txt", "d9.txt", "d10.txt" };

// Current working file
String File_To_Write;

// Number of SD_Card_File in SD Card
int file_count = 0;

/*============================================================================================================================================================================*/

// JSON Packet Count Number 
int pack_num = 0;

// JSON Packet Storage Counter
int pack_count = 0;

// JSON Packet Storage Limit
int pack_limit = 30;

/*============================================================================================================================================================================*/

// JSON Packet Sending time Counter
unsigned long timer = 30000; // 30 seconds

// Status Downtime Value for i.e. 33 Seconds
unsigned long DT = 33000;

// Increment time for each packet i.e. 5 minute
unsigned long inc_val = 30000; // 30 seconds

/*============================================================================================================================================================================*/

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS3 = 0;
int SS5 = 0;

// Sensor 1, 3, 5 Previous Status Bit
int PS1 = 0;
int PS3 = 0;
int PS5 = 0;

// Sensor`s Down Time Stamps for continuous low input 
unsigned long DTS1 = 0;
unsigned long DTS3 = 0;
unsigned long DTS5 = 0;

// Sensor`s Continuous Time Stamps for continuous high input 
unsigned long CTS1 = 0;
unsigned long CTS3 = 0;
unsigned long CTS5 = 0;

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

/*============================================================================================================================================================================*/

void setup() {
  Serial.begin(500000);   
  if (SD.begin(53)) { 
    Serial.println("SD Card Connected");
    digitalWrite(Q0_0, HIGH); 
    digitalWrite(Q0_1, HIGH); 
    delay(100);
    for (int i = 0; i < 10; i++) { //Serial.print("deleting all SD_Card_File"); Serial.println(SD_Card_File[i]); 
      SD.remove(SD_Card_File[i]); delay(500); }
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
      case 51:                  // Looping over an Array when Rx = 3
        Send_SD_Card_Data(); 
        break;
      case 52:                  // Indexing back to zero when Rx =4
        clear_SD_Card_Data();
        break;
    }
  }
  Input_Pins_Sensing();
  Time_And_Event_Driven_Arch();
}

/*============================================================================================================================================================================*/

void Input_Pins_Sensing() {
  
  // Sensor 1 ( Good Output Count and Status ) ..............................................
  S1 = digitalRead(I0_12);
  if ( S1 == 1 && P1 == 0 ) { count1++; SS1=0; CTS1 = millis(); }              
  if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; }    
  if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }              
  if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }    
  P1 = S1;

  // Sensor 2 ( Rejection Count ) ...........................................................
  S2 = digitalRead(I0_11); 
  if ( S2 == 1 && P2 == 0 ) { count2++; } 
  P2 = S2;

  // Sensor 3 ( Good Output Count and Status ) ..............................................
  S3 = digitalRead(I0_10);
  if ( S3 == 1 && P3 == 0 ) { count3++; SS3=0; CTS3 = millis(); } 
  if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; }
  if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > DT ) ) { SS3=1; }
  if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > DT ) ) { SS3=1; }
  P3 = S3;

  // Sensor 4 ( Rejection Count ) ...........................................................
  S4 = digitalRead(I0_9); 
  if ( S4 == 1 && P4 == 0 ) { count4++; } 
  P4 = S4;

  // Sensor 5 ( Good Output Count and Status ) .............................................
  S5 = digitalRead(I0_8);
  if ( S5 == 1 && P5 == 0 ) { count5++; SS5=0; CTS5 = millis(); } 
  if ( S5 == 0 && P5 == 1 ) { DTS5 = millis(); CTS5 = 0; }
  if ( S5 == 0 && P5 == 0 && ( ( millis() - DTS5 ) > DT ) ) { SS5=1; } 
  if ( S5 == 1 && P5 == 1 && ( ( millis() - CTS5 ) > DT ) ) { SS5=1; } 
  P5 = S5;

  // Sensor 6 ( Rejection Count ) ..........................................................
  S6 = digitalRead(I0_7); 
  if ( S6 == 1 && P6 == 0 ) { count6++; } 
  P6 = S6;
  
}

/*============================================================================================================================================================================*/

void Time_And_Event_Driven_Arch() {
  
  // Event trigger instantly if there is a change ..........................................
  if ( PS1 != SS1 )
  { 
    timer = millis()+inc_val;
    write_to_sd(); 
    PS1 = SS1;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS3 != SS3 )
  { 
    timer = millis()+inc_val;
    write_to_sd();  
    PS3 = SS3;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS5 != SS5 )
  { 
    timer = millis()+inc_val;
    write_to_sd(); 
    PS5 = SS5;
  }

  // JSON Packet Sent after every 5 min ....................................................
  if ( millis() >= timer )
  {                 
     timer = millis()+inc_val; 
     write_to_sd(); 
  }
  
}
/*============================================================================================================================================================================*/

void write_to_sd() {

  pack_count++;
  pack_num++;
  
  Set_Current_File();

  String pts = "\"PTS\":" + String(millis()) + "," ;
  String ptc = "\"PTC\":" + String(pack_num) + "," ;

  String sr1 = "\"SR1\":" + String(count1) + "," ;
  String sr2 = "\"SR2\":" + String(count2) + "," ;
  String sr3 = "\"SR3\":" + String(count3) + "," ;
  String sr4 = "\"SR4\":" + String(count4) + "," ;
  String sr5 = "\"SR5\":" + String(count5) + "," ;
  String sr6 = "\"SR6\":" + String(count6) + "," ;

  String ss1 = "\"SS1\":" + String(SS1) + "," ;
  String ss3 = "\"SS3\":" + String(SS3) + "," ;
  String ss5 = "\"SS5\":" + String(SS5) ;

  json = "{" + pts + ptc + sr1 + sr2 + sr3 + sr4 + sr5 + sr6 + ss1 + ss3 + ss5 + "}" ;

  Write_to_SD_Card();

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
}

/*============================================================================================================================================================================*/

void Write_to_SD_Card() {
  /* Write to SD Card data File */
  data_file = SD.open(File_To_Write, FILE_WRITE);
  delay(100);
  if (data_file) { data_file.print(json); data_file.print(","); delay(100); data_file.close(); delay(100); }
  delay(100);
}

/*============================================================================================================================================================================*/

void Send_SD_Card_Data() {
  if (SD.begin(53)) {
    delay(100);
    for (int i = 0; i < file_count; i++) { //Serial.print("Sending file "); Serial.println(String(SD_Card_File[i])); 
      data_file = SD.open(SD_Card_File[i]);
      delay(100);
      if (data_file) { 
        Serial.print("{\"values\":[");
        for (int n = 0 ; n < (data_file.size()) - 1 ; n++) { Serial.print(char(data_file.read())); }
        Serial.println("]}A");
        delay(100);
        data_file.close();
        delay(100);
      }
      delay(100);
    }
    Serial.println("");
  }
}

/*============================================================================================================================================================================*/

void clear_SD_Card_Data() {
  delay(100);
  for (int i = 0; i < file_count; i++) { //Serial.print("Deleting "); Serial.println(SD_Card_File[i]); 
    SD.remove(SD_Card_File[i]); delay(100); 
  }
  delay(100);
  pack_count = 0;
  delay(100);
}

/*============================================================================================================================================================================*/

void Set_Current_File() {
  if ( ( pack_count > 0 ) && ( pack_count < 31 ) ) {
    File_To_Write = SD_Card_File[0];
    file_count = 1;
  }
  if ( ( pack_count > 30 ) && ( pack_count < 61 ) ) {
    File_To_Write = SD_Card_File[1];
    file_count = 2;
  }
  if ( ( pack_count > 60 ) && ( pack_count < 91 ) ) {
    File_To_Write = SD_Card_File[2];
    file_count = 3;
  }
  if ( ( pack_count > 90 ) && ( pack_count < 121 ) ) {
    File_To_Write = SD_Card_File[3];
    file_count = 4;
  }
  if ( ( pack_count > 120 ) && ( pack_count < 151 ) ) {
    File_To_Write = SD_Card_File[4];
    file_count = 5;
  }
  if ( ( pack_count > 150 ) && ( pack_count < 181 ) ) {
    File_To_Write = SD_Card_File[5];
    file_count = 6;
  }
  if ( ( pack_count > 180 ) && ( pack_count < 211 ) ) {
    File_To_Write = SD_Card_File[6];
    file_count = 7;
  }
  if ( ( pack_count > 210 ) && ( pack_count < 241 ) ) {
    File_To_Write = SD_Card_File[7];
    file_count = 8;
  }
  if ( ( pack_count > 240 ) && ( pack_count < 271 ) ) {
    File_To_Write = SD_Card_File[8];
    file_count = 9;
  }
  if ( ( pack_count > 270 ) && ( pack_count < 301 ) ) {
    File_To_Write = SD_Card_File[9];
    file_count = 10;
  }
}
