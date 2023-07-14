#include <WiFi.h>
#include <WiFiMulti.h>

//=========================================================================================================================

const char* ssid = "Procheck Team";
const char* pass = "pc@54321";

WiFiMulti wifiMulti;

//=========================================================================================================================

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 10000;

//=========================================================================================================================

void setup(){
  
  Serial.begin(9600);
  delay(5000);
  
  Connect_To_Wifi();
  delay(5000);
  
}

//=========================================================================================================================

void loop(){
  
  //if the connection to the stongest hotstop is lost, it will connect to the next network on the list
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    Serial.print("WiFi connected: ");
    Serial.print(WiFi.SSID());
    Serial.print(" ");
    Serial.println(WiFi.RSSI());
  } else {
    Serial.println("WiFi not connected!");
  }
  
  delay(1000);
  
}

//=========================================================================================================================

void Connect_To_Wifi(){

  WiFi.mode(WIFI_STA);
  
  // Add list of wifi networks
  wifiMulti.addAP(ssid, pass);
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  // Connects to the SSID with strongest connection
  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi not connected!");
  }
  
}
