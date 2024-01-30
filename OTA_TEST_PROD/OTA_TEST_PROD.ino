#include "cert.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

//-------------------------------------------------------------------------------------------------------------------------------------------------

const char * ssid = "Cotbus Team";
const char * pass = "pc@54321";

//-------------------------------------------------------------------------------------------------------------------------------------------------

#define URL_fw_Bin "https://appsvclinuxcentralu9d0c.blob.core.windows.net/uploadfiles/testingnow_avi63.bin"

//-------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {

  Serial.begin(500000);
  
  pinMode(LED_BUILTIN, OUTPUT);
  connect_wifi();

  delay(5000);

  int i = 500;

  while ( i > 0 ) {
    digitalWrite(LED_BUILTIN, 1);
    delay(1500);
    digitalWrite(LED_BUILTIN, 0);
    delay(1500);
    Serial.println(i);
    i--;
  }
  
  Serial.println("Firmware update Starting..");
  firmwareUpdate();

  Serial.println("Firmware update done .... ");
  
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void loop() { }

//-------------------------------------------------------------------------------------------------------------------------------------------------

void connect_wifi() {
  
  Serial.println("Waiting for WiFi");
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void firmwareUpdate() {
  
  WiFiClientSecure client;
//  client.setInsecure();
  
  client.setCACert(rootCACertificate);
  
  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

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
