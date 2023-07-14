#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern "C" {
#include "crypto/base64.h"
}

const char* ssid = "Procheck Team";
const char* password = "pc@54321";

//Your Domain name with URL path or IP address with path
String URL_Link = "https://api.github.com/repos/samama-tariq/github_api_testing/contents/file.json";

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

    http.begin(URL_Link);  //Specify the URL
    http.addHeader("Content-Type", "application/json"); 
    http.addHeader("Accept", "application/vnd.github+json");
    http.addHeader("X-GitHub-Api-Version", "2022-11-28");
    http.addHeader("Authorization", "Bearer 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");
    
    int httpCode = http.GET();                                   //Make the request

    if (httpCode > 0) {  //Check for the returning code

      String payload = http.getString();
      Serial.println("HTTP Code : " + String(httpCode));
      Serial.println("Payload : " + String(payload));

      // Allocate Stack Memory for 1000 bytes
      StaticJsonDocument<1000> doc;
      DeserializationError error = deserializeJson(doc, payload);

      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      // JSON Data Parsing
      String Name = doc["name"];
      String Path = doc["path"];
      String Sha = doc["sha"];
      int Size = doc["size"];
      String Url = doc["url"];
      String Html_Url = doc["html_url"];
      String Git_Url = doc["git_url"];
      String Download_Url = doc["download_url"];
      String Type = doc["type"];
      String Content = doc["content"];
      String Encoding = doc["encoding"];

      // Print values
      Serial.println("Name : " + String(Name));
      Serial.println("Path : " + String(Path));
      Serial.println("Sha : " + String(Sha));
      Serial.println("Size : " + String(Size));
      Serial.println("Url : " + String(Url));
      Serial.println("Html_Url : " + String(Html_Url));
      Serial.println("Git_Url : " + String(Git_Url));
      Serial.println("Download_Url : " + String(Download_Url));
      Serial.println("Type : " + String(Type));
      Serial.println("Content : " + String(Content));
      Serial.println("Encoding : " + String(Encoding));

      char Buf[50];
      Content.toCharArray(Buf, 50);

      char * toDecode = Buf;
      size_t outputLength;
     
      unsigned char * decoded = base64_decode((const unsigned char *)toDecode, strlen(toDecode), &outputLength);
     
      Serial.print("Length of decoded message: ");
      Serial.println(outputLength);
     
      Serial.printf("%.*s", outputLength, decoded);
      free(decoded);
      
    }

    else {
      Serial.println("Error on HTTP request");
    }

    http.end();  //Free the resources
  }

  delay(30000);
}
