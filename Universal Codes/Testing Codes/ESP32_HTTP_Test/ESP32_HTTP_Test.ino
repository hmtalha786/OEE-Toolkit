#include <WiFi.h>
#include <ESP32Ping.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* ssid = "Procheck Team";
const char* pass = "pc@54321";

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//String URL = "https://procheck-prod.azure-devices.net/devices/automatic-car_uii25/messages/events?api-version=2018-06-30";
//String SAS = "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2Fautomatic-car_uii25&sig=nPqIcxwZOs0gNQekrcOkwUF6Am6RmCz%2F3wE67FHirws%3D&se=1709627084";

String URL = "https://pdm-prod.azure-devices.net/devices/testingCertificates/messages/events?api-version=2018-06-30";
String SAS = "SharedAccessSignature sr=pdm-prod.azure-devices.net%2Fdevices%2FtestingCertificates&sig=rit3AHhIYAmFjzQznIJHWu2YHQwJVYgPlbGnamF96fw%3D&se=1715670093";

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

String json;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {

  Serial.begin(9600);
  while (!Serial);

  //----------------------------------------------------------

  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // ---------------------------------------------------------

  /* Printing Network Credentials */
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

  // ---------------------------------------------------------
  
  /* Making Ping to Google to Check Internet Connection */
  bool Ping_Success = Ping.ping("www.google.com", 3);

  if (!Ping_Success) {
    Serial.println("Failed to Ping www.google.com");
  } else {
    Serial.println("Ping successful to www.google.com");
  }

  delay(5000);

  // ---------------------------------------------------------

  // Create a JSON object
  StaticJsonDocument<500> doc;

  doc["PTS"] = millis();
  doc["SR1"] = 22;
  doc["SR2"] = 22;
  doc["SR3"] = 22;
  doc["SR4"] = 22;
  doc["SS1"] = 0;
  doc["SS2"] = 0;
  doc["SS3"] = 0;
  doc["SS4"] = 0;

  // Serialize the JSON object to a string
  serializeJson(doc, json);
  Serial.println(json);
  delay(5000);
  
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {

  // Declare object of class HTTPClient
  HTTPClient https;
  https.begin(URL);
  https.addHeader("Authorization", SAS);
  https.addHeader("Content-Type", "application/json");              // Specify content-type header
  Serial.println(json);
  
  int httpCode = https.POST(json);                                  // Send the request
  Serial.println("HTTP Response Code = " + String(httpCode));       // Print HTTP return code
  https.end();

  delay(15000);
  
}
