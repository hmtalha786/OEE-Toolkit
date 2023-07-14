#include <WiFi.h>
#include <WiFiMulti.h>
#include <Esp32MQTTClient.h>

WiFiMulti wifiMulti;

unsigned long timer = 15000; // 15 Seconds

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 10000;

static const char* connectionString = "HostName=procheck-oee-uat.azure-devices.net;DeviceId=test101_ii7mm;SharedAccessKey=dj5+PLg1lCxXixjNluDOJq+3tOur8B5zszuTQuC1rto=";

static bool isHubConnect = false;

// ====================================================================================================================================================================================

void setup() {
  Serial.begin(500000);
  delay(10);
  WiFi.mode(WIFI_STA);

  // Add list of wifi networks
  wifiMulti.addAP("procheckTest", "Procheck@321");
  wifiMulti.addAP("Laptop", "LPC12345");
  wifiMulti.addAP("Realme", "2k17mte04");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  }
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }

  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString))
  {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  isHubConnect = true;
}

void loop() {
  
  //if the connection to the stongest hotstop is lost, it will connect to the next network on the list
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.RSSI());
  }
  else {
    Serial.println("WiFi not connected!");
  }

  delay(1000);

  /* Send Empty Packet in order to stay sync with azure iot hub */
  if ( millis() >= timer ) {

    /* Creating Empty Object */
    char buff[128];
    snprintf(buff, 128, "{}");
    Serial.println(buff);

    /* Sending Empty Packet to Azure IoT Hub */
    if (isHubConnect && Esp32MQTTClient_SendEvent(buff)) {
      Serial.println("Empty Packet Sent Successfully");

      /* Blink RGB Red */
      digitalWrite(32, 1);
      delay(100);
      digitalWrite(32, 0);
      delay(100);

      /* Blink RGB Green Led */
      digitalWrite(15, 1);
      delay(100);
      digitalWrite(15, 0);

    } else {

      Serial.println("Failure occur in sending to Azure IoT Hub");

      /* Blink RGB Blue Led on failure in data sending through mqtt */
      digitalWrite(33, 1);
      delay(300);
      digitalWrite(33, 0);
      delay(50);

      /* Restrart esp32 if there is a failure in send request */
      ESP.restart();
    }

    /* Increment Empty Packet Sending Time by 15 Seconds */
    timer = millis() + 15000UL;
    Serial1.write("3");
    Serial.println("");
    Serial.println("Sent fetch data signal to PLC");
    Serial.println("");
  }
}
