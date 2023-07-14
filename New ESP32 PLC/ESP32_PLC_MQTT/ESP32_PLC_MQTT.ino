#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include <ESP32Ping.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <Esp32MQTTClient.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define Pin_0 4
#define Pin_1 2
#define Pin_2 15
#define Pin_3 32
#define Pin_4 25

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define WDT_TIMEOUT 300  // 5 Minute

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// JSON Packet Serial Number 
int pack_num = 0;

// Check for IoT Hub Connection
static bool isHubConnect = false;

// WiFi connection time checker
unsigned long Time_Checker = 0;

// Timer Increment Value
unsigned long timer_inc = 15000;

// JSON Packet Sending time Counter
unsigned long timer = 15000;

// Downtime Value i.e. 33 Seconds
unsigned long DT = 33000;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor`s Status Bits ( 0 => ON , 1 => OFF )
int SS1 = 0;
int SS2 = 0;
int SS3 = 0;
int SS4 = 0;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Sensor Previous Status Bit
int PS1 = 0;
int PS2 = 0;
int PS3 = 0;
int PS4 = 0;

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

struct My_Object {
  char ssid[25];
  char pass[25];
  char cstr[150];
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

My_Object customVarr;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {

        /* ----- Make object from struct to carry credentials ----- */
        My_Object Credentials;
        char WiFi_SSID[25] = "";
        char WiFi_PASS[25] = "";
        char Conec_STR[150] = "";

        /* ----- Extract Connection String ----- */
        if ( value[0] == '@' ) {

          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            Serial.print(value[i]);
          }

          for (int i = 1; i < value.length(); i++) {
            Conec_STR[i - 1] = value[i];
          }

          Serial.print("Connection String : ");
          Serial.println(Conec_STR);
          memcpy(Credentials.cstr, Conec_STR, sizeof(Credentials.cstr));

        } else {
          int x;
          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            if ( value[i] == ',' ) {
              x = i;
            }
            Serial.print(value[i]);
          }
          Serial.println("");
          Serial.print("Comma is at index : ");
          Serial.println(x);

          /* ----- Extract WiFi SSID ----- */

          for (int i = 0; i < x; i++) {
            WiFi_SSID[i] = value[i];
          }
          Serial.print("WiFi_Username : ");
          Serial.println(WiFi_SSID);
          memcpy(Credentials.ssid, WiFi_SSID, sizeof(Credentials.ssid));

          /* ----- Extract WiFi Password ----- */

          for (int i = x + 1; i < value.length(); i++) {
            WiFi_PASS[i - (x + 1)] = value[i];
          }
          Serial.print("WiFi_Password : ");
          Serial.println(WiFi_PASS);
          memcpy(Credentials.pass, WiFi_PASS, sizeof(Credentials.pass));

          /* ----- Keep the Connection String Same ----- */

          EEPROM.get(0, customVarr);
          Serial.println(customVarr.cstr);
          memcpy(Credentials.cstr, customVarr.cstr, sizeof(Credentials.cstr));
        }

        /* ----- Store WiFi Credentials to EEPROM Address 0 ----- */
        EEPROM.put(0, Credentials);
        EEPROM.commit();
      }
    }
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);
  EEPROM.begin(500);
  delay(500);

  Serial.println("Configurating Watch Dog Timer ....");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  pinMode(Pin_0, INPUT);   // Bluetooth Switch
  pinMode(Pin_1, INPUT);   // Sensor 1
  pinMode(Pin_2, INPUT);   // Sensor 2
  pinMode(Pin_3, INPUT);   // Sensor 3
  pinMode(Pin_4, INPUT);   // Sensor 4

    /* ----- Retrieve WiFi Credentials from EEPROM ----- */
  EEPROM.get(0, customVarr);
  Serial.print("WiFi_Username : "); Serial.println(customVarr.ssid);
  Serial.print("WiFi_Password : "); Serial.println(customVarr.pass);
  Serial.print("Conection Str : "); Serial.println(customVarr.cstr);

  /* ----- Turn ON the bluetooth ----- */
  if (digitalRead(Pin_0) == HIGH) {
    BLEDevice::init("ProCheck WiFi Shield");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
  }

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore( Task1code, "Task1", 10000, NULL, 1, &Task1, 0);                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore( Task2code, "Task2", 10000, NULL, 1, &Task2, 1);
  delay(500); 
  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Task1code( void * pvParameters ){
  
  Serial.print("Sensor Detection running on core ");
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
  
  Serial.print("Data Sending running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){

    // Connect to WiFi through saved credentials .............................................
    if ( WiFi.status() != WL_CONNECTED ) { Connect_To_WiFi(customVarr.ssid, customVarr.pass); }
  
    // JSON Packet Sent after every 10 sec ...................................................          
    if ( millis() >= timer ) { timer = millis()+timer_inc; json_packet_sender(); }

    // Event trigger instantly if there is a change ..........................................
    if ( PS1 != SS1 ) { timer = millis()+timer_inc;  json_packet_sender(); PS1 = SS1; }
  
    // Event trigger instantly if there is a change ..........................................
    if ( PS3 != SS3 ) { timer = millis()+timer_inc;  json_packet_sender(); PS3 = SS3; }
    
  }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void json_packet_sender(){

  pack_num++;
  
  StaticJsonBuffer<500> JSON_Packet;   
  JsonObject& JSON_Entry = JSON_Packet.createObject();

  JSON_Entry["PTS"] = millis();
  JSON_Entry["PTC"] = pack_num;
  JSON_Entry["SR1"] = count1;
  JSON_Entry["SR2"] = count2;
  JSON_Entry["SR3"] = count3;
  JSON_Entry["SR4"] = count4;
  JSON_Entry["SS1"] = SS1;
  JSON_Entry["SS2"] = SS2;
  JSON_Entry["SS3"] = SS3;
  JSON_Entry["SS4"] = SS4;

  char JSONmessageBuffer[500];
  JSON_Entry.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.print(JSONmessageBuffer);

  /* Sending Empty Packet to Azure IoT Hub */
  if (isHubConnect && Esp32MQTTClient_SendEvent(JSONmessageBuffer)) {
    Serial.println("Packet Sent Successfully");
  } else {
    Serial.println("Failed to send Data Packet");
    WiFi.disconnect();
    Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  }
  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Connect_To_WiFi(const char * ssid, const char * pwd) {

  Time_Checker = millis();
  Serial.println();
  Serial.println("Connecting to WiFi network: " + String(ssid));
  Serial.println();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    /* Restart ESP32 if not connected for 5 Minutes */
    if (millis() - Time_Checker > 300000) {
      Serial.println("Reconnecting to WiFi");
      WiFi.disconnect();
      Connect_To_WiFi(customVarr.ssid, customVarr.pass);
    }
  }

  /* Printing Network Credentials */
  Serial.println("\n WiFi connected!");
  Serial.println("Local IP: " + String(WiFi.localIP()));
  Serial.println("Subnet Mask: " + String(WiFi.subnetMask()));
  Serial.println("Gateway IP: " + String(WiFi.gatewayIP()));
  Serial.println("PDNS: " + String(WiFi.dnsIP(0)));
  Serial.println("SDNS: " + String(WiFi.dnsIP(1)));
  Serial.println("RRSI: " + String(WiFi.RSSI()) + "db");

  /* Making Ping to Google to Check Internet Connection */
  bool Ping_Success = Ping.ping("www.google.com", 3);
  if (!Ping_Success) {
    Serial.println("Ping failed");
    Serial.println("Reconnecting to WiFi");
    WiFi.disconnect();
    Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  } else {
    Serial.println("Ping successful.");
    Serial.println();
  }

  /* ----- Connect to Azure IoT Hub through Connection String ----- */
  if (!Esp32MQTTClient_Init((const uint8_t*)customVarr.cstr)) {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    WiFi.disconnect();
    Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  }
  isHubConnect = true;
  Serial.println("Connection Established with Azure IoT Hub");

}
