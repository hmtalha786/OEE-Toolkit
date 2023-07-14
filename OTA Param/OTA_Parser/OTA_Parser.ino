#include <WiFi.h>
#include <EEPROM.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <Esp32MQTTClient.h>

//=========================================================================================================================

const char* ssid = "Procheck Team";
const char* pass = "pc@54321";

//=========================================================================================================================

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

//=========================================================================================================================

void setup() {

  Serial.begin(9600);
  EEPROM.begin(500);
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

  Connect_To_Wifi();
  delay(5000);

  Http_Request();
  delay(5000);

}

//=========================================================================================================================

void loop() {}

//=========================================================================================================================

void Connect_To_Wifi(){
  
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi ...");
  }

  Serial.println("Connected to the WiFi network");
  
}

//=========================================================================================================================

void Http_Request(){
  
  HTTPClient http;
  http.begin("https://procheck-oee-uat.azurewebsites.net/api/deviceInfo?code=LLxMTp4ZLmkW86oUM5CO1b-gkCmFPCZq5qXmYYXRn110AzFuDldgaQ==&lineId=test01_hb9sp");  // Specify the URL

  int httpCode = http.GET();                                   // Make the request

  if (httpCode > 0) {  // Check for the returning code
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    parser(payload);
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();  //Free the resources
}

//=========================================================================================================================

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

//  splitFloatValue(String(fvc));

  /* ----- Make object from struct to carry credentials ----- */
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

  if ( String(fvc).toFloat() == String(customVarr.fvc).toFloat() ) { Serial.println("Value is same"); } else  { Serial.println("Value is not same"); }

//  String cstr = "HostName="+ String(customVarr.aih) +".azure-devices.net;DeviceId=" + String(customVarr.did) + ";SharedAccessKey=" + String(customVarr.pky);
//  static const char* connectionString = cstr.c_str();
//  Serial.println(connectionString);
//
//  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString))
//  {
//    Serial.println("Initializing IoT hub failed.");
//    return;
//  }

//  if ( String(acs) == String(customVarr.acs) ) { Serial.print("dfsdfsdfsdfsdf"); }

}

//=========================================================================================================================

void splitFloatValue(const String& floatValue) {

  if ( floatValue.toFloat() == String(customVarr.fvc).toFloat() ) { Serial.println("Value is same"); }
  
  // Find the position of the decimal point
  int decimalPosition = floatValue.indexOf('.');
  
  // Extract the whole number part
  int wholeNumber = floatValue.substring(0, decimalPosition).toInt();
  
  // Extract the decimal part
  int decimalPart = floatValue.substring(decimalPosition + 1).toInt();

  Serial.print("Whole number part: ");
  Serial.println(wholeNumber);
  
  Serial.print("Decimal part: ");
  Serial.println(decimalPart);
  
}
