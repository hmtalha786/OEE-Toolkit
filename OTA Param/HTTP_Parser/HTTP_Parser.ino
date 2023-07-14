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
//  char url[150];
//  char sas[150];
  char acs[150];
  char wn1[15];
  char wn2[15];
  char wp1[15];
  char wp2[15];
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

  Connect_To_Wifi();
  delay(5000);

  Http_Request();
  delay(5000);

//  EEPROM.get(0, customVarr);
//  Serial.print("WiFi_1 : "); Serial.println(customVarr.wn1);
//  Serial.print("Pass_1 : "); Serial.println(customVarr.wp1);
//  Serial.print("WiFi_2 : "); Serial.println(customVarr.wn2);
//  Serial.print("Pass_2 : "); Serial.println(customVarr.wp2);
//  Serial.print("String : "); Serial.println(customVarr.acs);

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
//    parser(payload);
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
  const char* url = doc["URL"];
  const char* sas = doc["SAS"];
  const char* acs = doc["ACS"];
  const char* nsr = doc["NSR"];
  const char* esr = doc["ESR"];
  const char* mpl = doc["MPL"];
  const char* wn1 = doc["WN1"];
  const char* wp1 = doc["WP1"];
  const char* wn2 = doc["WN2"];
  const char* wp2 = doc["WP2"];
  const char* fvc = doc["FVC"];

//  splitFloatValue(String(fvc));

  // Print values
  Serial.println(url);
  Serial.println(sas);
  Serial.println(acs);
  Serial.println(nsr);
  Serial.println(esr);
  Serial.println(mpl);
  Serial.println(wn1);
  Serial.println(wp1);
  Serial.println(wn2);
  Serial.println(wp2);
  Serial.println(fvc);

  /* ----- Make object from struct to carry credentials ----- */
  My_Object Credentials;

//  memcpy(Credentials.url, url, sizeof(Credentials.url));
//  memcpy(Credentials.sas, sas, sizeof(Credentials.sas));
  memcpy(Credentials.acs, acs, sizeof(Credentials.acs));
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
//  Serial.println(customVarr.url);
//  Serial.println(customVarr.sas);
  Serial.println(customVarr.acs);
  Serial.println(customVarr.wn1);
  Serial.println(customVarr.wp1);
  Serial.println(customVarr.wn2);
  Serial.println(customVarr.wp2);
  Serial.println(customVarr.nsr);
  Serial.println(customVarr.esr);
  Serial.println(customVarr.mpl);
  Serial.println(customVarr.fvc);
  delay(5000);

//  if ( String(acs) == String(customVarr.acs) ) { Serial.print("dfsdfsdfsdfsdf"); }

}

//=========================================================================================================================

void splitFloatValue(const String& floatValue) {
  
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
