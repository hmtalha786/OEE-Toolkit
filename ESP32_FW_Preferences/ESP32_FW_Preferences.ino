#include "cert.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESP32Ping.h>
#include <HTTPUpdate.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <BluetoothSerial.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Preferences preferences;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FW_VERSION 2                  // OTA Firmware Version

#define WDT_TIMEOUT 500               // Watch Dog Timer set to >5 Minute

#define SERIAL_SIZE_RX  10000         // used in Serial.setRxBufferSize

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long api_call_timer = 900000;      // 15 Minute

unsigned long timer = 15000;                // 15 Seconds timer for data polling from PLC

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

String URL;
String SAS;
String API;
String SIG;
String DID;
String PKY;
String AIH;
String NSR;
String ESR;
String MPL;
String WN1;
String WN2;
String WP1;
String WP2;
String PVC;
String FVC;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PSF;  // Packet Sending Flag
int MEF;  // Memory Erase Flag

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(500000);
  Serial.setRxBufferSize(SERIAL_SIZE_RX);

  // ---------------------------------------------------------

  Serial1.begin(500000);
  Serial1.setRxBufferSize(SERIAL_SIZE_RX);

  // ---------------------------------------------------------
  
  Serial.println("Configurating Watch Dog Timer ....");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  // ---------------------------------------------------------

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(21, OUTPUT);  // BLE Switch
  pinMode(14, OUTPUT);  // Red WiFi LED
  pinMode(15, OUTPUT);  // Green RGB LED
  pinMode(32, OUTPUT);  // Red RGB LED
  pinMode(33, OUTPUT);  // Blue RGB LED

  // ---------------------------------------------------------

  preferences.begin("credentials", false);

  SIG = preferences.getString("SIG", ""); Serial.println("SIG : " + SIG);
  DID = preferences.getString("DID", ""); Serial.println("DID : " + DID);
  PKY = preferences.getString("PKY", ""); Serial.println("PKY : " + PKY);
  AIH = preferences.getString("AIH", ""); Serial.println("AIH : " + AIH);
  NSR = preferences.getString("NSR", ""); Serial.println("NSR : " + NSR);
  ESR = preferences.getString("ESR", ""); Serial.println("ESR : " + ESR);
  MPL = preferences.getString("MPL", ""); Serial.println("MPL : " + MPL);
  WN1 = preferences.getString("WN1", ""); Serial.println("WN1 : " + WN1);
  WN2 = preferences.getString("WN2", ""); Serial.println("WN2 : " + WN2);
  WP1 = preferences.getString("WP1", ""); Serial.println("WP1 : " + WP1);
  WP2 = preferences.getString("WP2", ""); Serial.println("WP2 : " + WP2);
  PVC = preferences.getString("PVC", ""); Serial.println("PVC : " + PVC);
  FVC = preferences.getString("FVC", ""); Serial.println("FVC : " + FVC);

  preferences.end();

  delay(5000);

  URL = "https://Cotbus-prod.azure-devices.net/devices/" + DID + "/messages/events?api-version=2020-03-13";
  Serial.println("HTTP Endpoint : " + URL );

  SAS = "SharedAccessSignature sr=Cotbus-prod.azure-devices.net%2Fdevices%2F" + DID + "&sig=" + SIG;
  Serial.println("Authorization : " + SAS );

  /* ----- Turn ON the Hardware Switch for BT & HW Serial ----- */
  if ( digitalRead(21) == HIGH ) {
    Serial_Input();
  }

  /* ----- Connect to WiFi through saved credentials ----- */
  Connect_To_WiFi();
  delay(5000);

  Serial.println("Firmware Version in the Code : " + String(FW_VERSION));

  /* ----- Check Firmware Version for OTA update ----- */
  if ( FVC.toInt() > String(FW_VERSION).toInt() ) {
    Serial.println("OTA Firmware Version Update Available");
    OTA_Update();
  }

  /* ----- Make HTTP Get Request for OTA Parameters ----- */
  HTTP_Get_Request_OTA();
  delay(5000);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  if ( WiFi.status() != WL_CONNECTED ) {
    WiFi.disconnect();
    Connect_To_WiFi();
  }

  //-----------------------------------------------------------------------------------------

  if ( millis() >= api_call_timer ) {

    /* ----- Check Firmware Version for OTA update ----- */
    if ( FVC.toInt() > String(FW_VERSION).toInt() ) {

      ESP.restart();              // ESP Restart if Firmware Version is Updated

    } else {

      HTTP_Get_Request_OTA();     // Calling OTA Parameter Update Function

    }

    api_call_timer = millis() + 900000UL;    // Increment timer by 15 minute

  }

  //-----------------------------------------------------------------------------------------

  /* Send Empty Packet in order to stay sync with azure iot hub */
  if ( millis() >= timer ) {

    digitalWrite(32, 1);
    delay(300);
    digitalWrite(32, 0);
    delay(300);
    digitalWrite(15, 1);
    delay(300);
    digitalWrite(15, 0);

    /* Increment Empty Packet Sending Time by 15 Seconds */
    timer = millis() + 15000UL;
    PSF = 1;
    MEF = 0;
    Send_Json_Packet();
    Serial.println("Sent fetch data signal to PLC");

  }

  /* Check for Serial input pin "Rx" */
  if (Serial1.available() > 0) {

    char bfr[3501];
    memset(bfr, 0, 3501);
    Serial1.readBytesUntil('~', bfr, 3500);
    Send_To_Azure(bfr);

  }

  delay(1000);
  esp_task_wdt_reset();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Send_To_Azure( char * val ) {

  Serial.println("Payload : " + String(val));

  delay(300);
  digitalWrite(32, 1);  // Red LED High
  delay(300);
  digitalWrite(32, 0);  // Red LED Low
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
    digitalWrite(15, 1);  // Green LED High
    delay(300);
    digitalWrite(15, 0);  // Green LED Low
    delay(300);

    /* ----- Data successfully sent signal to PLC ----- */
    PSF = 0;
    MEF = 1;
    Send_Json_Packet();
    Serial.println(" Sent Acknowledgement to PLC");
  }

  //-------------------------------------------------------------------------------

  if (httpCode == -1)    // Failed to Send Data
  {
    digitalWrite(33, 1);  // Blue LED High
    delay(300);
    digitalWrite(33, 0);  // Blue LED Low
    delay(300);
    WiFi.disconnect();
    Connect_To_WiFi();
  }

  //-------------------------------------------------------------------------------

  if (httpCode == 400 || httpCode == 401 )    // Authorization Error due to token expire
  {
    digitalWrite(33, 1);   delay(300);    digitalWrite(33, 0);  delay(300);
    digitalWrite(33, 1);   delay(300);    digitalWrite(33, 0);  delay(300);
    digitalWrite(33, 1);   delay(300);    digitalWrite(33, 0);  delay(300);
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Send_Json_Packet() {

  StaticJsonDocument<500> doc;

  doc["NSR"] = NSR.toInt();    // Normal Scan Rate
  doc["ESR"] = ESR.toInt();    // Extended Scan Rate
  doc["MPL"] = MPL.toInt();    // Maximum Packet Limit

  doc["PSF"] = PSF;            // Packet Sending Flag
  doc["MEF"] = MEF;            // Memory Erase Flag

  String json;
  serializeJson(doc, json);
  Serial.print(json);
  Serial1.print(json);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Connect_To_WiFi() {

  // Define hostname
  String hostname = "Cotbus WiFi Shield";
  WiFi.setHostname(hostname.c_str());

  Serial.println("Connecting to WiFi .......");

  // Add list of wifi networks
  WiFi.mode(WIFI_STA);
  
  wifiMulti.addAP( WN1.c_str(), WP1.c_str() );
  wifiMulti.addAP( WN2.c_str(), WP2.c_str() );

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Serial_Input() {

  SerialBT.begin("Cotbus WiFi Shield");
  Serial.println("Credentials Input Channel Activated");

  while (true) {

    // Hardware Serial .....................
    if ( Serial.available() > 0 ) {
      char bfr[5001];
      memset(bfr, 0, 5001);
      Serial.readBytesUntil('!', bfr, 5000);
      Serial.println(bfr);
      Json_Parser(bfr);
    }

    // Bluetooth Serial ......................
    if ( SerialBT.available() > 0 ) {
      char bfr[5001];
      memset(bfr, 0, 5001);
      SerialBT.readBytesUntil('!', bfr, 5000);
      Serial.println(bfr);
      Json_Parser(bfr);
    }

  }

  btStop();  // Turn off the bluetooth

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HTTP_Get_Request_OTA() {

  HTTPClient http;
  String ota_url = "https://Cotbusprodfunctions.azurewebsites.net/api/deviceInfo?code=FG7C97GGtLao1ulqxCnIsULvjOH71QWQF9_NYRl-yYkYAzFumZfCWw==&lineId=" + DID;
  Serial.println("OTA URL : " + ota_url);
  http.begin(ota_url);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
  const char* sig = doc["SIG"].isNull() ? "" : doc["SIG"].as<const char*>();
  const char* did = doc["DID"].isNull() ? "" : doc["DID"].as<const char*>();
  const char* pky = doc["PKY"].isNull() ? "" : doc["PKY"].as<const char*>();
  const char* aih = doc["AIH"].isNull() ? "" : doc["AIH"].as<const char*>();
  const char* nsr = doc["NSR"].isNull() ? "" : doc["NSR"].as<const char*>();
  const char* esr = doc["ESR"].isNull() ? "" : doc["ESR"].as<const char*>();
  const char* mpl = doc["MPL"].isNull() ? "" : doc["MPL"].as<const char*>();
  const char* wn1 = doc["WN1"].isNull() ? "" : doc["WN1"].as<const char*>();
  const char* wp1 = doc["WP1"].isNull() ? "" : doc["WP1"].as<const char*>();
  const char* wn2 = doc["WN2"].isNull() ? "" : doc["WN2"].as<const char*>();
  const char* wp2 = doc["WP2"].isNull() ? "" : doc["WP2"].as<const char*>();
  const char* pvc = doc["PVC"].isNull() ? "" : doc["PVC"].as<const char*>();
  const char* fvc = doc["FVC"].isNull() ? "" : doc["FVC"].as<const char*>();

  // Print values
  Serial.println(sig);
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
  Serial.println(pvc);
  Serial.println(fvc);

  /* ----- Check OTA Parameter Version ----- */
  if ( ( String(pvc).toInt() > PVC.toInt() ) || ( String(fvc).toInt() > FVC.toInt() ) ) {

    Serial.println("Parameter Version Update Available ... ");

    preferences.begin("credentials", false);

    preferences.putString("SIG", String(sig));
    preferences.putString("DID", String(did));
    preferences.putString("PKY", String(pky));
    preferences.putString("AIH", String(aih));
    preferences.putString("NSR", String(nsr));
    preferences.putString("ESR", String(esr));
    preferences.putString("MPL", String(mpl));
    preferences.putString("WN1", String(wn1));
    preferences.putString("WN2", String(wn2));
    preferences.putString("WP1", String(wp1));
    preferences.putString("WP2", String(wp2));
    preferences.putString("PVC", String(pvc));
    preferences.putString("FVC", String(fvc));

    preferences.end();

    Serial.println("Parameter Version Updated Sucessfully ... ");

    delay(5000);

    ESP.restart();    // Restart ESP

  } else {

    Serial.println("No Update Available ... ");
    
  }

}

// Firmware Update Process --------------------------------------------------------------------------------------------------------

void OTA_Update() {

  Serial.println("Starting OTA Firmware Update ... ");

  API = "https://appsvclinuxcentralu9d0c.blob.core.windows.net/uploadfiles/" + DID + ".bin";

  Serial.println("Firmware API : " + API );

  Serial.println("Going to Update Firmware ... ");

  delay(1000);

  WiFiClientSecure client;
  client.setCACert(rootCACertificate);

  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, API);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }

}
