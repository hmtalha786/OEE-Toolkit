#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Procheck Team";
const char* password = "pc@54321";

//Your Domain name with URL path or IP address with path
String url = "https://procheck-oee-uat.azurewebsites.net/api/deviceInfo?code=LLxMTp4ZLmkW86oUM5CO1b-gkCmFPCZq5qXmYYXRn110AzFuDldgaQ==&lineId=test01_hb9sp";

void setup() {

  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {

  if ((WiFi.status() == WL_CONNECTED)) {  //Check the current connection status

    HTTPClient http;

    http.begin(url);  //Specify the URL
    int httpCode = http.GET();                                   //Make the request

    if (httpCode > 0) {  //Check for the returning code

      String payload = http.getString();
      Serial.println("HTTP Code : " + String(httpCode));
      Serial.println("Payload : " + String(payload));

      // Allocate Stack Memory for 1000 bytes
//      StaticJsonDocument<1000> doc;
//      DeserializationError error = deserializeJson(doc, payload);
//
//      // Test if parsing succeeds.
//      if (error) {
//        Serial.print(F("deserializeJson() failed: "));
//        Serial.println(error.f_str());
//        return;
//      }
//
//      // JSON Data Parsing
//      String Name = doc["name"];
//      int Age = doc["age"];
//      int Status = doc["status"];
//
//      // Print values
//      Serial.println("Name : " + String(Name));
//      Serial.println("Age : " + String(Age));
//      Serial.println("Status : " + String(Status));
      
    }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end();  //Free the resources
  }

  delay(10000);
}
