#include <ArduinoJson.h>

// Buffer Array
String val[100];

// Array Index
int arr_index = 0;

// Array Read Status
bool flag = false;

// JSON Packet Sending time Counter
unsigned long timer = 1000; // 1 second

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;
int SS3 = 0;
int SS4 = 0;

// Sensor 1, 3, 5 Previous Status Bit
int PS1 = 0;
int PS3 = 0;

// Sensor`s Input Pin Values
int S1 = 0;
int S2 = 0;
int S3 = 0;
int S4 = 0;

// Sensor`s Input Pin Previous Values
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;

// Sensor`s Count Values
unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;

// Sensor`s Down Time Stamps for continuous low input
unsigned long DTS1 = 0;
unsigned long DTS2 = 0;
unsigned long DTS3 = 0;
unsigned long DTS4 = 0;

// Sensor`s Continuous Time Stamps for continuous high input
unsigned long CTS1 = 0;
unsigned long CTS2 = 0;
unsigned long CTS3 = 0;
unsigned long CTS4 = 0;

/*============================================================================================================================================================================*/

void setup() {
  Serial.begin(500000);
}

/*============================================================================================================================================================================*/

void loop() {

  // Read serial feedback from wifi shield .................................................
  if ( Serial.available() > 0 ) {
    switch (Serial.read()) {
      // Looping over an Array when Rx = 3
      case 51:
        if ( flag ) {
          Serial.print("{\"values\":[");
          for (int n = 0 ; n < arr_index; n++) {
            Serial.print(val[n]);
            if (n == (arr_index - 1)) {
              break;
            } else {
              Serial.print(",");
            }
          }
          Serial.print("]}");
          Serial.println("A");
        }
        break;
      // Indexing back to zero when Rx =4
      case 52:
        arr_index = 0;
        flag = false;
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

  // Event trigger instantly if there is a change ..........................................
  if ( PS1 != SS1 || PS3 != SS3 )
  {
    timer = millis() + 300000UL;
    store_data();
    PS1 = SS1;
    PS3 = SS3;
  }

  if ( millis() >= timer )
  {
    timer = millis() + 300000UL;
    store_data();
  }
}

/*============================================================================================================================================================================*/

void store_data() {

  // Declaring static JSON buffer ......................................................................
  StaticJsonBuffer<300> JSON_Packet;   

  // Creating JSON Object ..............................................................................
  JsonObject& JSON_Entry = JSON_Packet.createObject();
  
  // Declaring JSON Key Value Entries ..................................................................
  JSON_Entry["PTS"] = millis();
  JSON_Entry["SR1"] = count1;
  JSON_Entry["SR2"] = count2;
  JSON_Entry["SR3"] = count3;
  JSON_Entry["SR4"] = count4;
  JSON_Entry["SS1"] = SS1;
  JSON_Entry["SS2"] = SS2;
  JSON_Entry["SS3"] = SS3;
  JSON_Entry["SS4"] = SS4;

  char JSONmessageBuffer[300];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  // Converting JSON Object to String .................................................................
  String json_pack = JSONmessageBuffer;

  // Setting array value, flag and index ...............................................................
  val[arr_index] = json_pack;
  flag = true;
  arr_index++;

  // Backward Sorting Array to overcome packet overloading ...........................................
  if (arr_index > 45) {
    for (int n = 0 ; n < arr_index; n++) {
      if (n == (arr_index - 1)) {
        break;
      } else {
        val[n] = val[n + 1];
      }
    }
    arr_index--;
  }
}
