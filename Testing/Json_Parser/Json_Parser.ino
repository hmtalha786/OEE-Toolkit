#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "PC-1";
const char* pass = "pc@12345";

void setup() {

  Serial.begin(500000);
  delay(4000);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi ...");
  }

  Serial.println("Connected to the WiFi network");
}

void loop() {

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
