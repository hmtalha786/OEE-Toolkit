#include <WiFi.h>
#include <EEPROM.h>
#include <WiFiMulti.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <BluetoothSerial.h>
#include <Esp32MQTTClient.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WiFiMulti wifiMulti;

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 60000;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WDT_TIMEOUT 300          // Watch Dog Timer set to 5 Minute

#define SERIAL_SIZE_RX  10000    // used in Serial.setRxBufferSize

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct My_Object {
  char did[25];
  char pky[50];
  char aih[25];
  char wn1[25];
  char wn2[25];
  char wp1[25];
  char wp2[25];
  char nsr[5];
  char esr[5];
  char mpl[5];
  char fvc[5];
};

My_Object customVarr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool isHubConnect = false;

unsigned long timer = 15000; // 15 Seconds

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PLC Parameters
int PSF;
int MEF; 

/////////////////////////////////////////////////////////////////////////////////////

void setup() {
  
  Serial.begin(9600);
  Serial1.begin(500000);
  Serial.setRxBufferSize(SERIAL_SIZE_RX);
  Serial1.setRxBufferSize(SERIAL_SIZE_RX);
  EEPROM.begin(500);
  delay(5000);

  Serial.println("Configurating Watch Dog Timer ....");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  pinMode(21, OUTPUT);  // BLE Switch
  pinMode(14, OUTPUT);  // Red WiFi LED
  pinMode(15, OUTPUT);  // Green RGB LED
  pinMode(32, OUTPUT);  // Red RGB LED
  pinMode(33, OUTPUT);  // Blue RGB LED

  /* ----- Retrieve WiFi Credentials from EEPROM ----- */
  EEPROM.get(0, customVarr);
  Serial.print("DID : "); Serial.println(customVarr.did);
  Serial.print("PKY : "); Serial.println(customVarr.pky);
  Serial.print("AIH : "); Serial.println(customVarr.aih);
  Serial.print("NSR : "); Serial.println(customVarr.nsr);
  Serial.print("ESR : "); Serial.println(customVarr.esr);
  Serial.print("MPL : "); Serial.println(customVarr.mpl);
  Serial.print("WN1 : "); Serial.println(customVarr.wn1);
  Serial.print("WP1 : "); Serial.println(customVarr.wp1);
  Serial.print("WN2 : "); Serial.println(customVarr.wn2);
  Serial.print("WP2 : "); Serial.println(customVarr.wp2);
  delay(5000);

  /* ----- Turn ON the Hardware Switch for BT & HW Serial ----- */
  if (digitalRead(21) == HIGH) { Serial_Input(); }

  /* ----- Connect to WiFi through saved credentials ----- */
  Connect_To_WiFi();
  delay(5000);

  /* ----- Make HTTP Get Request for OTA Parameters ----- */
  HTTP_Get_Request_OTA();
  delay(5000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  
  if ( WiFi.status() != WL_CONNECTED ) {
    WiFi.disconnect();
    Connect_To_WiFi();
  }
  
  /* Send Empty Packet in order to stay sync with azure iot hub */
  if ( millis() >= timer ) {

    /* Creating Empty Object */
    char buff[128];
    snprintf(buff, 128, "{}");
    Serial.println(buff);

    /* Sending Empty Packet to Azure IoT Hub */
    if (isHubConnect && Esp32MQTTClient_SendEvent(buff)) {
      Serial.println("Empty Packet Sent Successfully");

      /* Blink RGB Red */
      digitalWrite(32, 1);
      delay(300);
      digitalWrite(32, 0);
      delay(300);

      /* Blink RGB Green Led */
      digitalWrite(15, 1);
      delay(300);
      digitalWrite(15, 0);

    } else {

      Serial.println("Failure occur in sending to Azure IoT Hub");

      /* Blink RGB Blue Led on failure in data sending through mqtt */
      digitalWrite(33, 1);
      delay(300);
      digitalWrite(33, 0);
      delay(300);

      /* Reconnect to wifi if there is a failure in send request */
      WiFi.disconnect();
      Connect_To_WiFi();
    }

    /* Increment Empty Packet Sending Time by 15 Seconds */
    timer = millis() + 15000UL;
    PSF = 1;
    MEF = 0;
    Send_Json_Packet();
    Serial.println("Sent fetch data signal to PLC");
  }

  /* Check for Serial input pin "Rx" */
  if (Serial1.available() > 0) {

    /* Create an Array as a buffer to store incoming serial data */
    char bfr[5001];
    memset(bfr, 0, 5001);

    /* readBytesUntil(Terminator, data, dataLength) */
    Serial1.readBytesUntil('A', bfr, 5000);
    Serial.println(bfr);

    /* Blink RGB Red on data recieve from Rx */
    digitalWrite(32, 1);
    delay(300);
    digitalWrite(32, 0);

    if (isHubConnect && Esp32MQTTClient_SendEvent(bfr)) {
      Serial.println("Successfully sent to Azure IoT Hub");

      /* Blink RGB Green Led on successfully data send through mqtt */
      digitalWrite(15, 1);
      delay(300);
      digitalWrite(15, 0);

      /* ----- Data successfully sent signal to PLC ----- */
      PSF = 0;
      MEF = 1;
      Send_Json_Packet();
      Serial.println("Sent confirmation signal to PLC");

    } else {

      Serial.println("Failure occur in sending to Azure IoT Hub");

      /* Blink RGB Blue Led on failure in data sending through mqtt */
      digitalWrite(33, 1);
      delay(300);
      digitalWrite(33, 0);
      delay(300);

      /* Reconnect to wifi if there is a failure in send request */
      WiFi.disconnect();
      Connect_To_WiFi();
    }
  }

  delay(1000);
  esp_task_wdt_reset();
  
}

/////////////////////////////////////////////////////////////////////////////////////

void Send_Json_Packet(){
  
  StaticJsonDocument<500> doc;
  
  doc["NSR"] = String(customVarr.nsr).toInt();    // Normal Scan Rate
  doc["ESR"] = String(customVarr.esr).toInt();    // Extended Scan Rate
  doc["MPL"] = String(customVarr.mpl).toInt();    // Maximum Packet Limit
  
  doc["PSF"] = PSF;    // Packet Sending Flag
  doc["MEF"] = MEF;    // Memory Erase Flag
  
  String json;
  serializeJson(doc, json);
  Serial.print(json);
  Serial1.print(json);
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connect_To_WiFi() {

  int ledState = 0;

  // Define hostname
  String hostname = "Procheck WiFi Shield";
  WiFi.setHostname(hostname.c_str()); 
  
  // Add list of wifi networks
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(customVarr.wn1, customVarr.wp1);
  wifiMulti.addAP(customVarr.wn2, customVarr.wp2);

  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    digitalWrite(14, HIGH);
    delay(500);
    digitalWrite(14, LOW);
    Serial.print(".");
  } else {
    WiFi.disconnect();
    ESP.restart();
  }

  digitalWrite(14, 0);
  Serial.println("");

  /* Printing Network Credentials */
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

  /* Making Ping to Google to Check Internet Connection */
  bool Ping_Success = Ping.ping("www.google.com", 3);

  if (!Ping_Success) {
    Serial.println("Failed to Ping www.google.com");
    WiFi.disconnect();
    Connect_To_WiFi();
  } else {
    Serial.println();
    Serial.println("Ping successful to www.google.com");
    Serial.println();
  }

  /* ----- Connect to Azure IoT Hub through Connection String ----- */
  String cstr = "HostName="+ String(customVarr.aih) +".azure-devices.net;DeviceId=" + String(customVarr.did) + ";SharedAccessKey=" + String(customVarr.pky);
  static const char* connectionString = cstr.c_str();
  Serial.println(connectionString);
  
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString)) {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    WiFi.disconnect();
    Connect_To_WiFi();
  }
  
  isHubConnect = true;
  Serial.println("Connection Established with Azure IoT Hub");
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HTTP_Get_Request_OTA() {
  
  HTTPClient http;
  String url = "https://" + String(customVarr.aih) + ".azurewebsites.net/api/deviceInfo?code=LLxMTp4ZLmkW86oUM5CO1b-gkCmFPCZq5qXmYYXRn110AzFuDldgaQ==&lineId=" + String(customVarr.did);
  Serial.println("OTA URL : " + String(url));
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    Json_Parser(payload);
  } else {
    Serial.println("Error on HTTP request");
  }
  
  http.end();  //Free the resources
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Serial_Input() {

  SerialBT.begin("Procheck WiFi Shield");
  Serial.println("Bluetooth Serial Activated");

  while ( true ) {

    // Hardware Serial .....................
    if (Serial.available()>0) {
      char bfr[5001];
      memset(bfr, 0, 5001);
      Serial.readBytesUntil('!', bfr, 5000);
      Serial.println(bfr);
      Json_Parser(bfr);
    }

    // Bluetooth Serial ......................
    if (SerialBT.available()>0) {
      char bfr[5001];
      memset(bfr, 0, 5001);
      SerialBT.readBytesUntil('!', bfr, 5000);
      Serial.println(bfr);
      Json_Parser(bfr);
    }
  
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Json_Parser(String Str) {

  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, Str);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("Deserialize Json() failed : "));
    Serial.println(error.f_str());
    return;
  }

  // JSON Data Parsing
  const char* did = doc["DID"];
  const char* pky = doc["PKY"];
  const char* aih = doc["AIH"];
  const char* nsr = doc["NSR"];
  const char* esr = doc["ESR"];
  const char* mpl = doc["MPL"];
  const char* wn1 = doc["WN1"];
  const char* wp1 = doc["WP1"];
  const char* wn2 = doc["WN2"];
  const char* wp2 = doc["WP2"];
  const char* fvc = doc["FVC"];

  // Print values
  Serial.println(did);
  Serial.println(pky);
  Serial.println(aih);
  Serial.println(nsr);
  Serial.println(esr);
  Serial.println(mpl);
  Serial.println(wn1);
  Serial.println(wp1);
  Serial.println(wn2);
  Serial.println(wp2);
  Serial.println(fvc);

  if ( String(fvc).toFloat() > String(customVarr.fvc).toFloat() ) { 
    
    Serial.println("Firmware Version is Update ... Going to Update EEPROM"); 
      
    My_Object Credentials;
    memcpy(Credentials.did, did, sizeof(Credentials.did));
    memcpy(Credentials.pky, pky, sizeof(Credentials.pky));
    memcpy(Credentials.aih, aih, sizeof(Credentials.aih));
    memcpy(Credentials.wn1, wn1, sizeof(Credentials.wn1));
    memcpy(Credentials.wn2, wn2, sizeof(Credentials.wn2));
    memcpy(Credentials.wp1, wp1, sizeof(Credentials.wp1));
    memcpy(Credentials.wp2, wp2, sizeof(Credentials.wp2));
    memcpy(Credentials.nsr, nsr, sizeof(Credentials.nsr));
    memcpy(Credentials.esr, esr, sizeof(Credentials.esr));
    memcpy(Credentials.mpl, mpl, sizeof(Credentials.mpl));
    memcpy(Credentials.fvc, fvc, sizeof(Credentials.fvc));
    EEPROM.put(0, Credentials);
    EEPROM.commit();
    delay(5000);
  
    EEPROM.get(0, customVarr);
    Serial.println(customVarr.did);
    Serial.println(customVarr.pky);
    Serial.println(customVarr.aih);
    Serial.println(customVarr.wn1);
    Serial.println(customVarr.wp1);
    Serial.println(customVarr.wn2);
    Serial.println(customVarr.wp2);
    Serial.println(customVarr.nsr);
    Serial.println(customVarr.esr);
    Serial.println(customVarr.mpl);
    Serial.println(customVarr.fvc);
    delay(5000);
    
  } else { 

    if ( String(fvc).toFloat() == String(customVarr.fvc).toFloat() ) { 
      Serial.println("Value is Same"); 
    } else { 
      Serial.println("API Version is Outdated");
    }
    
  }

}
