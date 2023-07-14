#include <WiFi.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define Pin_0 4
#define Pin_1 2
#define Pin_2 15
#define Pin_3 32
#define Pin_4 25

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define WDT_TIMEOUT 300  // 5 Minute

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* ssid = "Procheck Team";
const char* pass = "pc@54321";

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

String SAS_Token = "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2Fline-1_05vg3&sig=z5jz8z1wKXY%2FhSTq6hCzzDLa7LVfbzO4YWaJrdHF2tE%3D&se=1708499673";
String Azure_URL = "https://procheck-prod.azure-devices.net/devices/line-1_05vg3/messages/events?api-version=2020-03-13";

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// WiFi connection time checker
unsigned long Time_Checker = 0;

// JSON Packet Sending time Counter ( After every 10 sec )
unsigned long previous_time = 10000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1;
int SS2;
int SS3;
int SS4;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor`s Input Pin Values
int S1 = 0;
int S2 = 0;
int S3 = 0;
int S4 = 0;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor`s Input Pin Previous Values
int P1 = 0;
int P2 = 0;
int P3 = 0;
int P4 = 0;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor`s Count Values 
unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor`s Down Time Stamps for continuous low input 
unsigned long DTS1 = 0;
unsigned long DTS2 = 0;
unsigned long DTS3 = 0;
unsigned long DTS4 = 0;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor`s Continuous Time Stamps for continuous high input 
unsigned long CTS1 = 0;
unsigned long CTS2 = 0;
unsigned long CTS3 = 0;
unsigned long CTS4 = 0;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TaskHandle_t Task1;
TaskHandle_t Task2;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);

  pinMode(Pin_0, INPUT);
  pinMode(Pin_1, INPUT);
  pinMode(Pin_2, INPUT);
  pinMode(Pin_3, INPUT);
  pinMode(Pin_4, INPUT);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore( Task1code, "Task1", 10000, NULL, 1, &Task1, 0);                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore( Task2code, "Task2", 10000, NULL, 1, &Task2, 1);
  delay(500); 

  Serial.println("Configurating Watch Dog Timer ....");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    
    // Sensor 1 ..............................................................................
    S1 = digitalRead(Pin_1);
    if ( S1 == 1 && P1 == 0 ) { count1++; SS1=0; CTS1 = millis(); }              
    if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; }                      
    if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }         
    if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }         
    P1 = S1;
  
    // Sensor 2 ..............................................................................
    S2 = digitalRead(Pin_2);
    if ( S2 == 1 && P2 == 0 ) { count2++; SS2=0; CTS2 = millis(); } 
    if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; }
    if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
    if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
    P2 = S2;
  
    // Sensor 3 ..............................................................................
    S3 = digitalRead(Pin_3);
    if ( S3 == 1 && P3 == 0 ) { count3++; SS3=0; CTS3 = millis(); } 
    if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; }
    if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > DT ) ) { SS3=1; }
    if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > DT ) ) { SS3=1; }
    P3 = S3;
  
    // Sensor 4 ..............................................................................
    S4 = digitalRead(Pin_4);
    if ( S4 == 1 && P4 == 0 ) { count4++; SS4=0; CTS4 = millis(); } 
    if ( S4 == 0 && P4 == 1 ) { DTS4 = millis(); CTS4 = 0; }
    if ( S4 == 0 && P4 == 0 && ( ( millis() - DTS4 ) > DT ) ) { SS4=1; } 
    if ( S4 == 1 && P4 == 1 && ( ( millis() - CTS4 ) > DT ) ) { SS4=1; } 
    P4 = S4;
  
  }
  esp_task_wdt_reset();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){

    if ( WiFi.status() != WL_CONNECTED ) {
      /* ----- Connect to WiFi through saved credentials ----- */
      Connect_WiFi_Ping(ssid, pass);
    }
  
    // JSON Packet Sent after every 10 sec ...................................................          
    if ( millis() >= previous_time ) { previous_time = millis()+10000UL; json_packet_sender(); }

  }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void json_packet_sender(){
  
  // Declaring static JSON buffer ......................................................................
  StaticJsonBuffer<500> JSON_Packet;   

  // Creating JSON Object ..............................................................................
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  // Declaring JSON Key Value Pairs of Sensor Counts ...................................................
  JSON_Entry["SR1"] = count1;
  JSON_Entry["SR2"] = count2;
  JSON_Entry["SR3"] = count3;
  JSON_Entry["SR4"] = count4;

  // Declaring JSON Key Value Pairs of Sensor Status ...................................................
  JSON_Entry["SS1"] = SS1;
  JSON_Entry["SS2"] = SS2;
  JSON_Entry["SS3"] = SS3;
  JSON_Entry["SS4"] = SS4;

  char JSONmessageBuffer[500];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  String body = JSONmessageBuffer;
  Serial.print(body);

  HTTPClient http;
  http.begin(Azure_URL);                                     // Specify destination for HTTP request
  http.addHeader("Content-Type", "application/json");        // Specify content-type header
  http.addHeader("Authorization", SAS_Token);

  int httpResponseCode = http.POST(body);                    // Send the actual POST request

  if (httpResponseCode > 0) {
    String response = http.getString();                      // Get the response to the request
    Serial.println(httpResponseCode);                        // Print return code
    Serial.println(response);                                // Print request answer
  } else {
    Serial.print("Error on sending to Azure IoT Hub : ");
    Serial.println(httpResponseCode);
    Serial.println("Reconnecting to WiFi and Getting Azure Signature");
    WiFi.disconnect();
    Connect_WiFi_Ping(ssid, pass);
  }
  
  http.end();    // Free resources
  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connect_WiFi_Ping(const char * ssid, const char * pwd) {

  // Connect to WiFi and print credentials ..........................................................................................

  int ledState = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  Time_Checker = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - Time_Checker > 300000) {
      Serial.println("Reconnecting to WiFi");
      WiFi.disconnect();
      Connect_WiFi_Ping(ssid, pass);
    }
  }

  Serial.println();

  // Print Network credentials .....................................................................................................

  Serial.println("WiFi connected!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Primary DNS: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("Secondary DNS: ");
  Serial.println(WiFi.dnsIP(1));

  // Ping Google for checking internet .............................................................................................

  bool Ping_Success = Ping.ping("www.google.com", 3);

  if (!Ping_Success) {
    Serial.println("Ping failed");
    Serial.println("Reconnecting to WiFi");
    WiFi.disconnect();
    Connect_WiFi_Ping(ssid, pass);
  } else {
    Serial.println("Ping successful.");
    Serial.println();
  }

}
