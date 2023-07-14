#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned long WiFi_Time_Check = 0;
unsigned long timer = 15000; // 15 Seconds

const char* ssid = "MBL";
const char* pass = "PassWord";

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

String URL = "https://procheck-prod.azure-devices.net/devices/testingnow_avi63/messages/events?api-version=2018-06-30";
String SAS = "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2Ftestingnow_avi63&sig=%2FwoDaFeJ0nxympVlNR0m3klffnjBzGI4jmZE7hoeqb4%3D&se=1709888593";

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {

  Serial.begin(500000);
  delay(5000);
  Serial1.begin(500000);
  pinMode(14, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(21, OUTPUT);

  connectToWiFi(ssid, pass);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {

  if ( millis() >= timer ) {
    digitalWrite(15, 1);
    delay(100);
    digitalWrite(15, 0);
    delay(100);
    timer = millis() + 15000UL;
    Serial1.write("3");
    Serial.println("Sent fetch data signal to PLC");
  }

  if (Serial1.available() > 0) {
    char bfr[501];
    Serial.println("------------------");
    memset(bfr, 0, 501);
    Serial1.readBytesUntil('A', bfr, 500);

    digitalWrite(32, 1);
    delay(300);
    digitalWrite(32, 0);

    // Declare object of class HTTPClient
    HTTPClient https;
    https.begin(URL);
    https.addHeader("Authorization", SAS);
    https.addHeader("Content-Type", "application/json");    // Specify content-type header
    Serial.println(bfr);

    int httpCode = https.POST(bfr);                                   // Send the request
    Serial.println("HTTP Response Code = " + String(httpCode));       // Print HTTP return code
    https.end();

    if (httpCode == 200 || httpCode == 204) {
      digitalWrite(15, 1);
      delay(300);
      digitalWrite(15, 0);
      delay(300);
      Serial1.write("4");
      Serial.println("Sent confirmation signal to PLC");
    }

    if (httpCode == -1)
    {
      digitalWrite(33, 1);
      delay(300);
      digitalWrite(33, 0);
      delay(300);
      Serial.println("HTTP Request Failed");
      delay(300);
      connectToWiFi(ssid, pass);
    }

    if (httpCode == 400 || httpCode == 401 ) // SAS Token
    {
      digitalWrite(33, 1);
      delay(300);
      digitalWrite(33, 0);
      delay(300);
      Serial.println("SAS Token Expired");
      delay(300);
      connectToWiFi(ssid, pass);
    }
  }

}

// =================================================================================================================================================================================

void connectToWiFi(const char * ssid, const char * pwd) {

  int ledState = 0;
  Serial.println("-------------------------------------------------------------------------");
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi_Time_Check = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);
    ledState = (ledState + 1) % 2; // Flip ledState
    delay(500);
    Serial.print(".");
    if (millis() - WiFi_Time_Check > 300000) {
      ESP.restart();
    }
  }

  digitalWrite(14, 1);

  Serial.println("");
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
  delay(5000);
}
