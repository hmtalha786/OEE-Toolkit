/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------- Libraries ---------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <WiFi.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------ Inputs / Outputs Pins -------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#define Sensor_1 I0_12
#define Sensor_2 I0_11
#define Sensor_3 I0_10
#define Sensor_4 I0_9
#define Sensor_5 I0_8
#define Sensor_6 I0_7

#define WiFi_LED Q0_4
#define Data_LED Q0_2
#define Fail_LED Q0_0

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------- WiFi and Azure Credentials -----------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

const char* ssid = "Procheck";
const char* pass = "Procheck@123";

String Device_ID = "testingnow_avi63";
String Primary_Key = "GdbbD6RWnNdj1/gio34aVQYf/fsQq5G8awGBNpaR1A4=";

String SAS_Token;
String SAS_Signature;
String Azure_URL;

unsigned long UTC_Time;

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------- Global Variables ---------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// WiFi connection time checker
unsigned long Time_Checker = 0;

// JSON Packet Sending time Counter
unsigned long timer = 30000;

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

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------ Task Handlers ---------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

TaskHandle_t Task1;
TaskHandle_t Task2;

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------- Void Setup -----------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void setup() {
  
  Serial.begin(115200);
  delay(500);

  /* ----- Connect to WiFi through saved credentials ----- */
  Connect_WiFi_Ping(ssid, pass);

  /* --------- Generate SAS Token and Azure URLs ----------*/
  Generate_SAS_Token();

  // create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);

}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------- Void Loop ----------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void loop() {}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------- Thread 1 ( Sensor pins inputs ) ------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void Task1code( void * pvParameters ) {
  
  Serial.print("Sensor Pins Detection on Core ");
  Serial.println(xPortGetCoreID());
  
  while (true) {
  
    // Sensor 1 ..............................................................................
    S1 = digitalRead(Sensor_1);
    if ( S1 == 1 && P1 == 0 ) { count1++; SS1=0; CTS1 = millis(); }              
    if ( S1 == 0 && P1 == 1 ) { DTS1 = millis(); CTS1 = 0; }                      
    if ( S1 == 0 && P1 == 0 && ( ( millis() - DTS1 ) > DT ) ) { SS1=1; }         
    if ( S1 == 1 && P1 == 1 && ( ( millis() - CTS1 ) > DT ) ) { SS1=1; }         
    P1 = S1;
  
    // Sensor 2 ..............................................................................
    S2 = digitalRead(Sensor_2);
    if ( S2 == 1 && P2 == 0 ) { count2++; SS2=0; CTS2 = millis(); } 
    if ( S2 == 0 && P2 == 1 ) { DTS2 = millis(); CTS2 = 0; }
    if ( S2 == 0 && P2 == 0 && ( ( millis() - DTS2 ) > DT ) ) { SS2=1; }
    if ( S2 == 1 && P2 == 1 && ( ( millis() - CTS2 ) > DT ) ) { SS2=1; }
    P2 = S2;
  
    // Sensor 3 ..............................................................................
    S3 = digitalRead(Sensor_3);
    if ( S3 == 1 && P3 == 0 ) { count3++; SS3=0; CTS3 = millis(); } 
    if ( S3 == 0 && P3 == 1 ) { DTS3 = millis(); CTS3 = 0; }
    if ( S3 == 0 && P3 == 0 && ( ( millis() - DTS3 ) > DT ) ) { SS3=1; }
    if ( S3 == 1 && P3 == 1 && ( ( millis() - CTS3 ) > DT ) ) { SS3=1; }
    P3 = S3;
  
    // Sensor 4 ..............................................................................
    S4 = digitalRead(Sensor_4);
    if ( S4 == 1 && P4 == 0 ) { count4++; SS4=0; CTS4 = millis(); } 
    if ( S4 == 0 && P4 == 1 ) { DTS4 = millis(); CTS4 = 0; }
    if ( S4 == 0 && P4 == 0 && ( ( millis() - DTS4 ) > DT ) ) { SS4=1; } 
    if ( S4 == 1 && P4 == 1 && ( ( millis() - CTS4 ) > DT ) ) { SS4=1; } 
    P4 = S4;

    // Sensor 5 ..............................................................................
    S5 = digitalRead(Sensor_5);
    if ( S5 == 1 && P5 == 0 ) { count5++; SS5=0; CTS5 = millis(); } 
    if ( S5 == 0 && P5 == 1 ) { DTS5 = millis(); CTS5 = 0; }
    if ( S5 == 0 && P5 == 0 && ( ( millis() - DTS5 ) > DT ) ) { SS5=1; } 
    if ( S5 == 1 && P5 == 1 && ( ( millis() - CTS5 ) > DT ) ) { SS5=1; } 
    P5 = S5;
  
    // Sensor 6 ..............................................................................
    S6 = digitalRead(Sensor_6);
    if ( S6 == 1 && P6 == 0 ) { count6++; SS6=0; CTS6 = millis(); } 
    if ( S6 == 0 && P6 == 1 ) { DTS6 = millis(); CTS6 = 0; }
    if ( S6 == 0 && P6 == 0 && ( ( millis() - DTS6 ) > DT ) ) { SS6=1; } 
    if ( S6 == 1 && P6 == 1 && ( ( millis() - CTS6 ) > DT ) ) { SS6=1; } 
    P6 = S6;
  }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------ Thread 2 ( Time and Event Driven Architecture ) -----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void Task2code( void * pvParameters ) {
  
  Serial.print("Data sending to Azure IoT Hub on Core ");
  Serial.println(xPortGetCoreID());
  
  while (true) {
    
    // Event Driven Architecture .............................
    if( PS1 != SS1 || PS3 != SS3 || PS5 != SS5 ){ 
      timer = millis()+300000UL; 
      send_to_azure(); 
      PS1 = SS1; 
      PS3 = SS3; 
      PS5 = SS5; 
    }
    
    // Time Driven Architecture ..............................       
    if ( millis() >= timer ) { 
      timer = millis()+300000UL; 
      send_to_azure(); 
    }
       
  }
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------- Send To Azure Function --------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void send_to_azure() {
  
  StaticJsonBuffer<500> JSON_Packet;   
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["PTS"] = millis();
  JSON_Entry["UTS"] = UTC_Time + ( millis() / 1000UL );
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

  char JSONmessageBuffer[500];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  
  String body = JSONmessageBuffer;
  Serial.print(body);

  HTTPClient http;
  http.begin(Azure_URL);                                     // Specify destination for HTTP request

  http.addHeader("Content-Type", "application/json");        // Specify content-type header
  http.addHeader("Authorization", SAS_Signature);

  int httpResponseCode = http.POST(body);                    // Send the actual POST request

  if (httpResponseCode > 0) {
    
    String response = http.getString();                      // Get the response to the request
    Serial.println(httpResponseCode);                        // Print return code
    Serial.println(response);                                // Print request answer
    
    digitalWrite(Data_LED, 1);
    delay(300);
    digitalWrite(Data_LED, 0);
  
  } else {
    
    Serial.print("Error on sending to Azure IoT Hub : ");
    Serial.println(httpResponseCode);
    
    digitalWrite(Fail_LED, 1);
    delay(300);
    digitalWrite(Fail_LED, 0);
    
    Serial.println("Reconnecting to WiFi and Getting Azure Signature");
    WiFi.disconnect();
    Connect_WiFi_Ping(ssid, pass);
    Generate_SAS_Token();
    
  }
  
  http.end();    // Free resources
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------- Connect To WiFi, Generate SAS Token and Azure URLs Function ----------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void Connect_WiFi_Ping(const char * ssid, const char * pwd) {

  // Connect to WiFi and print credentials ..........................................................................................

  int ledState = 0;
  Serial.println("Connecting to WiFi network: " + String(ssid));
  Time_Checker = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(WiFi_LED, ledState);                 /* Blink LED while connecting to WiFi */
    ledState = (ledState + 1) % 2;                /* Flip LED State */
    delay(500);
    Serial.print(".");
    if (millis() - Time_Checker > 300000) {
      Serial.println("Reconnecting to WiFi");
      WiFi.disconnect();
      Connect_WiFi_Ping(ssid, pass);
    }
  }

  digitalWrite(WiFi_LED, 1);
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

  bool success = Ping.ping("www.google.com", 3);

  if (!success) {
    Serial.println("Ping failed");
    Serial.println("Reconnecting to WiFi");
    WiFi.disconnect();
    Connect_WiFi_Ping(ssid, pass);
  } else {
    Serial.println("Ping successful.");
    Serial.println();
  }

  // Get Unix Timestamp and set RTC with it ........................................................................................

  Serial.println("Configuring RTC via Internet Time : ");
  configTime(0, 0, "pool.ntp.org");
  UTC_Time = get_internet_time(); 
  Serial.print(" ("); Serial.print(UTC_Time); Serial.println(")"); 
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------- Generate Azure URLs and SAS Function ---------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void Generate_SAS_Token() {

  /* Fetch SAS Token and Generate SAS Signature */
  /* Create HTTP Client Instance */
  HTTPClient http;
  http.begin("https://procheckprodfunctions.azurewebsites.net/api/GetSasToken?code=4ji8Wv3sHFDHRaoGgcjhzZos6IaWZmwgdUW2POuVUtBXRBY5%2F0MbSw%3D%3D");
  http.addHeader("Content-Type", "application/json");

  /*Create JSON Request Body */
  StaticJsonBuffer<300> JSON_Packet;
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["id"] = String(Device_ID);
  JSON_Entry["key"] = String(Primary_Key);

  char JSONmessageBuffer[300];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  String request_body = JSONmessageBuffer;
  
  Serial.print("Request Body for fetching SAS Token : ");
  Serial.println(request_body);

  /* Make HTTP Post Request to fetch SAS Token */
  int httpResponseCode = http.POST(request_body);

  /* Handle HTTP Response i.e. Payload which is SAS Token */
  if (httpResponseCode > 0) {
    String response = http.getString();                      // Get the response to the request
    Serial.print("HTTP Response Code : ");
    Serial.println(httpResponseCode);                        // Print return code
    Serial.println(response);                                // Print request answer

    /* Fetched SAS Token */
    SAS_Token = String(response);
    Serial.print("SAS_Token : ");
    Serial.println(SAS_Token);

    /* Whole SAS Signature for HTTP Authorization */
    SAS_Signature = "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2F" + String(Device_ID) + "&sig=" + SAS_Token;
    Serial.print("SAS_Signature : ");
    Serial.println(SAS_Signature);

    /*Azure IoT Hub HTTP Endpoint for making Post Request*/
    Azure_URL = "https://procheck-prod.azure-devices.net/devices/" + String(Device_ID) + "/messages/events?api-version=2020-03-13";
    Serial.print("Azure IoT Hub Url : ");
    Serial.println(Azure_URL);

  } else {
    Serial.print("Error on getting SAS Token: ");
    Serial.println(httpResponseCode);
    
    Serial.println("Reconnecting to WiFi and Getting Azure Signature");
    WiFi.disconnect();
    Connect_WiFi_Ping(ssid, pass);
    Generate_SAS_Token();
  }

  http.end();
  
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------ Get Internet Time Function --------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

unsigned long get_internet_time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    ESP.restart();
  }
  Serial.print("Internet Timestamp: ");
  Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  time(&now);
  return now;
}
