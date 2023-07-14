#include "RTClib.h"

RTC_DS3231 rtc;

/*============================================================================================================================================================================*/

// JSON Packet Count Number 
int pack_num = 0;

// JSON Packet Sending time Counter
unsigned long timer = 30000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

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
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
}

/*============================================================================================================================================================================*/

void loop() {
  
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

  // Event trigger instantly if there is a change ..........................................
  if ( PS1 != SS1 )
  { 
    timer = millis()+300000UL;
    json_packet_sender(); 
    PS1 = SS1;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS3 != SS3 )
  { 
    timer = millis()+300000UL;
    json_packet_sender();  
    PS3 = SS3;
  }

  // Event trigger instantly if there is a change ..........................................
  if ( PS5 != SS5 )
  { 
    timer = millis()+300000UL;
    json_packet_sender(); 
    PS5 = SS5;
  }

  // JSON Packet Sent after every 5 min ....................................................
  if ( millis() >= timer )
  {                 
     timer = millis()+300000UL; 
     json_packet_sender(); 
  }

}

/*============================================================================================================================================================================*/

void json_packet_sender(){

  DateTime now = rtc.now();
  pack_num++;

  String uts = "\"UTS\":" + String(now.unixtime()-17940UL) + "," ;    // Coverting to 0 GMT
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

  String json = "{" + uts + pts + ptc + sr1 + sr2 + sr3 + sr4 + sr5 + sr6 + ss1 + ss3 + ss5 + "}" ;

  Serial.print(json);
  Serial.println('A');

  digitalWrite(Q0_4, HIGH);
  delay(50);
  digitalWrite(Q0_4, LOW);
}
