#include <WiFi.h>
#include "Esp32MQTTClient.h"

const char* ssid = "mespack2";
const char* pass = "Procheck123";
static const char* connectionString = "HostName=procheck-oee-uat.azure-devices.net;DeviceId=test101_ii7mm;SharedAccessKey=dj5+PLg1lCxXixjNluDOJq+3tOur8B5zszuTQuC1rto=";

static bool hasIoTHub = false;

// ====================================================================================================================================================================================

void setup() {
  Serial.begin(500000);
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

// =====================================================================================================================================================================================

void loop() {
  Serial.println("start sending events.");

  if (hasIoTHub)
  {
    char buff[128];

    // replace the following line with your data sent to Azure IoTHub
    snprintf(buff, 128, "{\"hello\":\"world\", \"UTS\":\"1669881278\"}");

    if (Esp32MQTTClient_SendEvent(buff))
    {
      Serial.println("Sending data succeed");
    }
    else
    {
      Serial.println("Failure...");
    }
    delay(5000);
  }
}
