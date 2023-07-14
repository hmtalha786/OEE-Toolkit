#include <WiFi.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BluetoothSerial.h>

//---------------------------------------------------------------------------------------------------------------

struct My_Object {
  char url[150];
  char sas[150];
  char nsr[15];
  char esr[15];
  char mpl[15];
  char wn1[15];
  char wn2[15];
  char wp1[15];
  char wp2[15];
};

//---------------------------------------------------------------------------------------------------------------

My_Object customVarr;

//---------------------------------------------------------------------------------------------------------------

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//---------------------------------------------------------------------------------------------------------------

BluetoothSerial SerialBT;

//---------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  SerialBT.begin("Procheck_Test");       //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  delay(500);
  EEPROM.begin(500);
  delay(500);
  /* ----- Retrieve WiFi Credentials from EEPROM ----- */
  EEPROM.get(0, customVarr);
  Serial.print("URL : "); Serial.println(customVarr.url);
  Serial.print("SAS : "); Serial.println(customVarr.sas);
  Serial.print("NSR : "); Serial.println(customVarr.nsr);
  Serial.print("ESR : "); Serial.println(customVarr.esr);
  Serial.print("MPL : "); Serial.println(customVarr.mpl);
  Serial.print("WN1 : "); Serial.println(customVarr.wn1);
  Serial.print("WN2 : "); Serial.println(customVarr.wn2);
  Serial.print("WP1 : "); Serial.println(customVarr.wp1);
  Serial.print("WP2 : "); Serial.println(customVarr.wp2);
  delay(500);
  WiFi.begin(customVarr.wn1, customVarr.wp1);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi ...");
  }

  Serial.println("Connected to the WiFi network");
}

//---------------------------------------------------------------------------------------------------------------

void loop() {

  if (SerialBT.available()) {

    char bfr[501];
    memset(bfr, 0, 501);

    /* readBytesUntil(Terminator, data, dataLength) */
    SerialBT.readBytesUntil('!', bfr, 500);
    Serial.println(bfr);

    // Allocate Stack Memory for 1000 bytes
    StaticJsonDocument<1000> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, bfr);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    delay(100);

    My_Object Credentials;
    char URLX[150] = "";
    char SASX[150] = "";
    char NSRX[15] = "";
    char ESRX[15] = "";
    char MPLX[15] = "";
    char WN1X[15] = "";
    char WN2X[15] = "";
    char WP1X[15] = "";
    char WP2X[15] = "";

    delay(100); 
    
    // JSON Data Parsing
    String URL = doc["url"];     // Full HTTP Azure Endpoint
    String SAS = doc["sas"];     // Full SAS Token
    String NSR = doc["nsr"];     // Normal Scan Rate
    String ESR = doc["esr"];     // Extended Scan Rate
    String MPL = doc["mpl"];     // Max Packet Limit
    String WN1 = doc["wn1"];     // Wifi Network 1
    String WN2 = doc["wn2"];     // Wifi Network 2
    String WP1 = doc["wp1"];     // Wifi Password 1
    String WP2 = doc["wp2"];     // Wifi Password 2

    delay(100);
    
    // Print values
    Serial.print("URL : ");   Serial.println(URL);
    Serial.print("SAS : ");   Serial.println(SAS);
    Serial.print("NSR : ");   Serial.println(NSR);
    Serial.print("ESR : ");   Serial.println(ESR);
    Serial.print("MPL : ");   Serial.println(MPL);
    Serial.print("WN1 : ");   Serial.println(WN1);
    Serial.print("WN2 : ");   Serial.println(WN2);
    Serial.print("WP1 : ");   Serial.println(WP1);
    Serial.print("WP2 : ");   Serial.println(WP2);

    delay(100);
    
    URL.toCharArray(URLX, 150);
    SAS.toCharArray(SASX, 150);
    NSR.toCharArray(NSRX, 15);
    ESR.toCharArray(ESRX, 15);
    MPL.toCharArray(MPLX, 15);
    WN1.toCharArray(WN1X, 15);
    WN2.toCharArray(WN2X, 15);
    WP1.toCharArray(WP1X, 15);
    WP2.toCharArray(WP2X, 15);

    delay(100);
    
    memcpy(Credentials.url, URLX, sizeof(Credentials.url));
    memcpy(Credentials.sas, SASX, sizeof(Credentials.sas));
    memcpy(Credentials.nsr, NSRX, sizeof(Credentials.nsr));
    memcpy(Credentials.esr, ESRX, sizeof(Credentials.esr));
    memcpy(Credentials.mpl, MPLX, sizeof(Credentials.mpl));
    memcpy(Credentials.wn1, WN1X, sizeof(Credentials.wn1));
    memcpy(Credentials.wn2, WN2X, sizeof(Credentials.wn2));
    memcpy(Credentials.wp1, WP1X, sizeof(Credentials.wp1));
    memcpy(Credentials.wp2, WP2X, sizeof(Credentials.wp2));
    EEPROM.put(0, Credentials);
    EEPROM.commit();
  }

  delay(100);

  if ((WiFi.status() == WL_CONNECTED)) {  //Check the current connection status

    HTTPClient http;

    http.begin("https://jsonplaceholder.typicode.com/posts/1");  //Specify the URL
    int httpCode = http.GET();                                   //Make the request

    if (httpCode > 0) {  //Check for the returning code

      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);

      // Allocate Stack Memory for 1000 bytes
      StaticJsonDocument<1000> doc;

      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, payload);

      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      // JSON Data Parsing
      int uid = doc["userId"];
      int id = doc["id"];
      const char* title = doc["title"];
      const char* body = doc["body"];

      // Print values
      Serial.println(uid);
      Serial.println(id);
      Serial.println(title);
      Serial.println(body);
    }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end();  //Free the resources
  }

  delay(10000);

}
