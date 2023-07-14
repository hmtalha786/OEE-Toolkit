#include <WiFi.h>
#include <EEPROM.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <esp_task_wdt.h>

// ==============================================================================================================================================================================================

#define WDT_TIMEOUT 300          // 5 Minute

#define SERIAL_SIZE_RX  10000    // used in Serial.setRxBufferSize

unsigned long timer = 15000;     // 15 Seconds

// ==============================================================================================================================================================================================

String SAS_URL = "https://procheckprodfunctions.azurewebsites.net/api/GetSasToken?code=4ji8Wv3sHFDHRaoGgcjhzZos6IaWZmwgdUW2POuVUtBXRBY5%2F0MbSw%3D%3D";

// ==============================================================================================================================================================================================

String URL;
String SAS;

// ==============================================================================================================================================================================================

struct My_Object {
  char ssid[15];
  char pass[15];
  char deid[30];
  char pkey[70];
  char ssig[100];
};

// ==============================================================================================================================================================================================

My_Object customVarr;

// ==============================================================================================================================================================================================

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ==============================================================================================================================================================================================

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {

        /* ----- Make object from struct to carry credentials ----- */
        My_Object Credentials;
        char WiFi_SSID[15] = "";
        char WiFi_PASS[15] = "";
        char Device_ID[30] = "";
        char Primary_Key[70] = "";
        char SAS_Token[100] = "";

        Serial.print("Entered value: ");
        for (int i = 0; i < value.length(); i++) { Serial.print(value[i]);  }
        Serial.println("");

        if ( value[0] == ':' ) {                                   // Device ID ______________________________________________________

          for (int i = 1; i < value.length(); i++) { Device_ID[i - 1] = value[i]; }
          Serial.println("Device ID : " + String(Device_ID));
          
          /* ----- Keep the other credentials Same ----- */
          EEPROM.get(0, customVarr);
          memcpy(Credentials.ssid, customVarr.ssid, sizeof(Credentials.ssid));
          memcpy(Credentials.pass, customVarr.pass, sizeof(Credentials.pass));
          memcpy(Credentials.deid, Device_ID, sizeof(Credentials.deid));
          memcpy(Credentials.pkey, customVarr.pkey, sizeof(Credentials.pkey));
          memcpy(Credentials.ssig, customVarr.ssig, sizeof(Credentials.ssig));

        } else if ( value[0] == '=' ) {                            // Primary Key ______________________________________________________

          for (int i = 1; i < value.length(); i++) { Primary_Key[i - 1] = value[i]; }
          Serial.println("Primary Key : " + String(Primary_Key));
          
          /* ----- Keep the other credentials Same ----- */
          EEPROM.get(0, customVarr);
          memcpy(Credentials.ssid, customVarr.ssid, sizeof(Credentials.ssid));
          memcpy(Credentials.pass, customVarr.pass, sizeof(Credentials.pass));
          memcpy(Credentials.deid, customVarr.deid, sizeof(Credentials.deid));
          memcpy(Credentials.pkey, Primary_Key, sizeof(Credentials.pkey));
          memcpy(Credentials.ssig, customVarr.ssig, sizeof(Credentials.ssig));

        } else if ( value[0] == '#' ) {                            // SAS Token ______________________________________________________

          for (int i = 1; i < value.length(); i++) { SAS_Token[i - 1] = value[i]; }
          Serial.println("SAS Signature : " + String(SAS_Token));
          
          /* ----- Keep the other credentials Same ----- */
          EEPROM.get(0, customVarr);
          memcpy(Credentials.ssid, customVarr.ssid, sizeof(Credentials.ssid));
          memcpy(Credentials.pass, customVarr.pass, sizeof(Credentials.pass));
          memcpy(Credentials.deid, customVarr.deid, sizeof(Credentials.deid));
          memcpy(Credentials.pkey, customVarr.pkey, sizeof(Credentials.pkey));
          memcpy(Credentials.ssig, SAS_Token, sizeof(Credentials.ssig));

        } else {                                                   // WiFi Credentials ___________________________________________________
          
          int x;
          for (int i = 0; i < value.length(); i++) {  if ( value[i] == ',' ) { x = i; }  }
          Serial.println("\nComma is at index : " + String(x));

          /* ----- Extract WiFi SSID ----- */
          for (int i = 0; i < x; i++) { WiFi_SSID[i] = value[i]; }
          Serial.println("WiFi_Username : " + String(WiFi_SSID));

          /* ----- Extract WiFi Password ----- */
          for (int i = x + 1; i < value.length(); i++) { WiFi_PASS[i - (x + 1)] = value[i]; }
          Serial.println("WiFi_Password : " + String(WiFi_PASS));
          
          /* ----- Keep the other credentials Same ----- */
          EEPROM.get(0, customVarr);
          memcpy(Credentials.ssid, WiFi_SSID, sizeof(Credentials.ssid));
          memcpy(Credentials.pass, WiFi_PASS, sizeof(Credentials.pass));
          memcpy(Credentials.deid, customVarr.deid, sizeof(Credentials.deid));
          memcpy(Credentials.pkey, customVarr.pkey, sizeof(Credentials.pkey));
          memcpy(Credentials.ssig, customVarr.ssig, sizeof(Credentials.ssig));
        }

        /* ----- Store Credentials to EEPROM Address 0 ----- */
        EEPROM.put(0, Credentials);
        EEPROM.commit();
      }
    }

};

/* =============================================================================================================================================================================== */

void setup() {
  
  Serial.begin(500000);
  Serial.setRxBufferSize(SERIAL_SIZE_RX);
  Serial1.begin(500000);
  Serial1.setRxBufferSize(SERIAL_SIZE_RX);

  // ---------------------------------------------------------
  
  Serial.println("Configurating Watch Dog Timer");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  // ---------------------------------------------------------
  
  pinMode(21, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(33, OUTPUT);

  // ---------------------------------------------------------

  EEPROM.begin(500);
  delay(5000);
  
  EEPROM.get(0, customVarr);
  Serial.println("************************EEPROM-START*****************************");
  Serial.println("WiFi SSID : " + String(customVarr.ssid));
  Serial.println("WiFi PASS : " + String(customVarr.pass));
  Serial.println("Device ID : " + String(customVarr.deid));
  Serial.println("Prime Key : " + String(customVarr.pkey));
  Serial.println("SAS Token : " + String(customVarr.ssig));
  Serial.println("************************EEPROM-END*******************************");

  // ---------------------------------------------------------
  
  URL = "https://procheck-prod.azure-devices.net/devices/" + String(customVarr.deid) + "/messages/events?api-version=2020-03-13";
  Serial.println("HTTP Endpoint : " + URL );

  SAS = "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2F" + String(customVarr.deid) + "&sig=" + String(customVarr.ssig);
  Serial.println("Authorization : " + SAS );
  
  // ---------------------------------------------------------
  
  if ( digitalRead(21) == HIGH ) {
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

  Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  
}

/* =============================================================================================================================================================================== */

void loop() {

  while (WiFi.status() == WL_CONNECTED) {

    if ( millis() >= timer ) {
      Send_To_Azure("{}");
      Serial1.write("3");
      timer = millis() + 30000UL;
      Serial.println("Sent fetch data signal to PLC");
    }
    
    if ( Serial1.available() > 0 ) {
      char bfr[1001];
      memset(bfr, 0, 1001);
      Serial1.readBytesUntil('A', bfr, 1000);
      Send_To_Azure(bfr);
      Serial1.write("4");
      Serial.println("Sent confirmation signal to PLC");
    }

    delay(1000);
    esp_task_wdt_reset();
  }

  WiFi.disconnect();
  Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  
}

/* =============================================================================================================================================================================== */

void Send_To_Azure( char * val ){

  Print_Line();
  Serial.println("Payload : " + String(val));
  
  delay(300);
  digitalWrite(32, 1);  
  delay(300);  
  digitalWrite(32, 0);
  delay(300);

  HTTPClient https;
  https.begin(URL);
  https.addHeader("Authorization", SAS);
  https.addHeader("Content-Type", "application/json");              // Specify content-type header

  int httpCode = https.POST(val);                                   // Send the request
  Serial.println("HTTP Response Code = " + String(httpCode));       // Print HTTP return code
  https.end();

  //-------------------------------------------------------------------------------

  if (httpCode == 200 || httpCode == 204)          // Successfully Sent
  {
    digitalWrite(15, 1);  
    delay(300);  
    digitalWrite(15, 0);  
  }              

  //-------------------------------------------------------------------------------

  if (httpCode == -1)                              // Failed to Sent
  {  
    digitalWrite(33, 1);  
    delay(300);  
    digitalWrite(33, 0);  
    delay(300);  
    WiFi.disconnect();
    Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  }      

  //-------------------------------------------------------------------------------

  if (httpCode == 400 || httpCode == 401 )        // Authorization Error
  {
    digitalWrite(33, 1);   delay(300);    digitalWrite(33, 0);  delay(300);
    digitalWrite(33, 1);   delay(300);    digitalWrite(33, 0);  delay(300);
    digitalWrite(33, 1);   delay(300);    digitalWrite(33, 0);  delay(300);
    Get_SAS_Token();
    delay(60000);
    ESP.restart();
  }
      
}

/* =============================================================================================================================================================================== */

void Get_SAS_Token() {

    HTTPClient http;
    http.begin(SAS_URL);
    http.addHeader("Content-Type", "application/json");
    
    String pack = "{\"id\":\"" + String(customVarr.deid) + "\", " + "\"key\":\"" + String(customVarr.pkey) + "\"}";
    Serial.println(pack);
    
    int httpCode = http.POST(pack);         // Send the request and get the response code
    String payload = http.getString();      // Get the response payload

    Serial.println("Response Code = " + String(httpCode));
    Serial.println("Payload = " + String(payload));
    http.end();

    char Temp_SAS [100];
    payload.toCharArray(Temp_SAS, 100);

    if ( httpCode == 200 || httpCode == 204 )
    {
      My_Object Obj;
     
      memcpy(Obj.ssid, customVarr.ssid, sizeof(Obj.ssid));
      memcpy(Obj.pass, customVarr.pass, sizeof(Obj.pass));
      memcpy(Obj.deid, customVarr.deid, sizeof(Obj.deid));
      memcpy(Obj.pkey, customVarr.pkey, sizeof(Obj.pkey));
      memcpy(Obj.ssig, Temp_SAS, sizeof(Obj.ssig));
     
      EEPROM.put(0, Obj);
      EEPROM.commit();
      Serial.println("Commited to the EEPROM Successfully");
    } else {
      WiFi.disconnect();
      Connect_To_WiFi(customVarr.ssid, customVarr.pass);
      Get_SAS_Token();
    }   
  
}

/* =============================================================================================================================================================================== */

void Connect_To_WiFi(const char * ssid, const char * pwd) {

  //-------------------------------------------------------------------------------
  
  int ledState = 0;
  unsigned long WiFi_Time_Check = millis();

  //-------------------------------------------------------------------------------
  
  Print_Line();

  //define hostname
  String hostname = "Procheck WiFi Shield";
  WiFi.setHostname(hostname.c_str()); 
  
  Serial.print("Connecting to WiFi network: " + String(ssid));
  WiFi.begin(ssid, pwd);

  //-------------------------------------------------------------------------------
  
  while (WiFi.status() != WL_CONNECTED) {
    
    digitalWrite(14, ledState);
    
    ledState = ( ledState + 1 ) % 2;     // Flip led State
    delay(500);
    Serial.print(".");
    
    if ( ( millis() - WiFi_Time_Check ) > 900000 ) { ESP.restart(); }
    
  }

  digitalWrite(14, 0);
  Serial.println("");
  
  //-------------------------------------------------------------------------------
  
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
  Serial.print("MAC Address:  ");
  Serial.println(WiFi.macAddress());
  Serial.print("RRSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println("db");
  Print_Line();
  
  //-------------------------------------------------------------------------------
  
  bool Ping_Success = Ping.ping("www.google.com", 3);

  if (Ping_Success) {
    Serial.println("Ping successful to www.google.com");
  } else {
    Serial.println("Failed to Ping www.google.com");
    WiFi.disconnect();
    Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  }

  delay(5000);
}

// =================================================================================================================================================================================

void Print_Line() {
  for (int i = 0; i < 50; i++) { Serial.print("-"); }
  Serial.println();
}
