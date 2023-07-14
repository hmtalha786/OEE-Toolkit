#include <WiFi.h>
#include "Esp32MQTTClient.h"

const char* ssid = "Procheck Team";
const char* pass = "pc@54321";
//static const char* connectionString = "HostName=procheck-prod.azure-devices.net;DeviceId=line-1_05vg3;SharedAccessKey=pPx9Db9dBbjJynNP8DcV2msnZCdtYcZmgJoQXsOeFGA=";

static const char* connectionString = "HostName=pdm-prod.azure-devices.net;DeviceId=testingCertificates;SharedAccessKey=gScNljwSHwebHjCEvdCXHC91fV3iPzmwy4UkUSVfI9o=";
static bool hasIoTHub = false;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);
  Serial.println("Starting connecting WiFi.");
  delay(10);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
  
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {

  if (hasIoTHub)
  {
    char buff[128];
    
    snprintf(buff, 128, "{\"topic\":\"iot\"}");     // replace the following line with your data sent to Azure IoTHub

    if (Esp32MQTTClient_SendEvent(buff)) {  Serial.println("Sending data succeed");  } else {  Serial.println("Failure...");  }
    
    delay(10000);
  }
  
}
