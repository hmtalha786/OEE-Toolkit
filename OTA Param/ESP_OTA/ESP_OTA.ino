#include <WiFi.h>
#include <EEPROM.h>
#include <WiFiMulti.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <Esp32MQTTClient.h>

/////////////////////////////////////////////////////////////////////////////////////

/* ------------ Watch Dog Timer ---------------- */

#define WDT_TIMEOUT 300          // 5 Minute

#define SERIAL_SIZE_RX  10000    // used in Serial.setRxBufferSize

/////////////////////////////////////////////////////////////////////////////////////

const char* ssid = "Procheck WiFi";
const char* pass = "pc@54321";

/////////////////////////////////////////////////////////////////////////////////////

String URL = "https://procheck-oee-uat.azurewebsites.net/api/deviceInfo?code=LLxMTp4ZLmkW86oUM5CO1b-gkCmFPCZq5qXmYYXRn110AzFuDldgaQ==&lineId=test01_hb9sp";

/////////////////////////////////////////////////////////////////////////////////////

static bool isHubConnect = false;

unsigned long timer = 15000; // 15 Seconds

/////////////////////////////////////////////////////////////////////////////////////

WiFiMulti wifiMulti;

/////////////////////////////////////////////////////////////////////////////////////

struct My_Object {
  char acs[150];
  char nsr[5];
  char esr[5];
  char mpl[5];
  char wn1[25];
  char wn2[25];
  char wp1[25];
  char wp2[25];
};

/////////////////////////////////////////////////////////////////////////////////////

My_Object customVarr;

/////////////////////////////////////////////////////////////////////////////////////

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 60000;

/////////////////////////////////////////////////////////////////////////////////////
  
// PLC Parameters
const char* NSR;
const char* ESR;
const char* MPL;
int PSF;
int MEF; 

/////////////////////////////////////////////////////////////////////////////////////

void setup(){
  
  Serial.begin(500000);
  Serial.setRxBufferSize(SERIAL_SIZE_RX);
  
  Serial1.begin(500000);
  Serial1.setRxBufferSize(SERIAL_SIZE_RX);
  
  EEPROM.begin(500);
  delay(5000);

  Serial.println("Configurating Watch Dog Timer ....");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  pinMode(21, OUTPUT);     // BLE Switch
  pinMode(14, OUTPUT);     // Red WiFi LED
  pinMode(15, OUTPUT);     // Green RGB LED
  pinMode(32, OUTPUT);     // Red RGB LED
  pinMode(33, OUTPUT);     // Blue RGB LED

  EEPROM.get(0, customVarr);
  NSR = customVarr.nsr;
  ESR = customVarr.esr;
  MPL = customVarr.mpl;
  
  Serial.print("ACS : "); Serial.println(customVarr.acs);
  Serial.print("NSR : "); Serial.println(customVarr.nsr);
  Serial.print("ESR : "); Serial.println(customVarr.esr);
  Serial.print("MPL : "); Serial.println(customVarr.mpl);
  Serial.print("WN1 : "); Serial.println(customVarr.wn1);
  Serial.print("WP1 : "); Serial.println(customVarr.wp1);
  Serial.print("WN2 : "); Serial.println(customVarr.wn2);
  Serial.print("WP2 : "); Serial.println(customVarr.wp2);
  delay(5000);
  
  Connect_To_WiFi();
  delay(5000);

  if (digitalRead(21) == HIGH) {
    Http_Request();
    delay(5000);
  }

  /* ----- Connect to Azure IoT Hub through Connection String ----- */
  if (!Esp32MQTTClient_Init((const uint8_t*)customVarr.acs)) {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    WiFi.disconnect();
    Connect_To_WiFi();
  }
  isHubConnect = true;
  Serial.println("Connection Established with Azure IoT Hub");
  
}

/////////////////////////////////////////////////////////////////////////////////////

void loop(){

  if ( WiFi.status() != WL_CONNECTED ) {
    WiFi.disconnect();
    Connect_To_WiFi();
  }
  
  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------    Send Empty Packet to Azure IoT Hub and Fetch Data signal to PLC   -----------------------------------------------------------------*/
  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

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

  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------------  Read Serial data from PLC  -------------------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

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
  
  doc["NSR"] = NSR;    // Normal Scan Rate
  doc["ESR"] = ESR;    // Extended Scan Rate
  doc["MPL"] = MPL;    // Maximum Packet Limit
  doc["PSF"] = PSF;    // Packet Sending Flag
  doc["MEF"] = MEF;    // Memory Erase Flag
  
  String json;
  serializeJson(doc, json);
  Serial.print(json);
  Serial1.print(json);
  
}

/////////////////////////////////////////////////////////////////////////////////////

void Connect_To_WiFi(){

  int ledState = 0;

  // Define hostname
  String hostname = "Procheck WiFi Shield";
  WiFi.setHostname(hostname.c_str()); 
  
  // Add list of wifi networks
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid, pass);
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
  
}

/////////////////////////////////////////////////////////////////////////////////////

void Http_Request(){
  
  HTTPClient http;
  http.begin(URL);                  // Specify the URL

  int httpCode = http.GET();        // Make the request

  if (httpCode > 0) {               // Check for the returning code
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    parser(payload);
  } else {
    Serial.println("Error on HTTP request");
  }
  
  http.end();                     // Free the resources
  
}

/////////////////////////////////////////////////////////////////////////////////////

void parser(String Str) {

  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, Str);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // JSON Data Parsing
  const char* ACS = doc["ACS"];
  const char* NSR = doc["NSR"];
  const char* ESR = doc["ESR"];
  const char* MPL = doc["MPL"];
  const char* WN1 = doc["WN1"];
  const char* WP1 = doc["WP1"];
  const char* WN2 = doc["WN2"];
  const char* WP2 = doc["WP2"];

  // Print values
  Serial.println("ACS : " + String(ACS));
  Serial.println("NSR : " + String(NSR));
  Serial.println("ESR : " + String(ESR));
  Serial.println("MPL : " + String(MPL));
  Serial.println("WN1 : " + String(WN1));
  Serial.println("WP1 : " + String(WP1));
  Serial.println("WN2 : " + String(WN2));
  Serial.println("WP2 : " + String(WP2));

  // Make object from struct to carry credentials
  My_Object Credentials;
  memcpy(Credentials.acs, ACS, sizeof(Credentials.acs));
  memcpy(Credentials.nsr, NSR, sizeof(Credentials.nsr));
  memcpy(Credentials.esr, ESR, sizeof(Credentials.esr));
  memcpy(Credentials.mpl, MPL, sizeof(Credentials.mpl));
  memcpy(Credentials.wn1, WN1, sizeof(Credentials.wn1));
  memcpy(Credentials.wn2, WN2, sizeof(Credentials.wn2));
  memcpy(Credentials.wp1, WP1, sizeof(Credentials.wp1));
  memcpy(Credentials.wp2, WP2, sizeof(Credentials.wp2));
  
  EEPROM.put(0, Credentials);
  EEPROM.commit();
  delay(5000);

}
