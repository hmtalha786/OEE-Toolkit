#include <WiFi.h>
#include <EEPROM.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ESP32Ping.h>
#include <esp_task_wdt.h>
#include <Esp32MQTTClient.h>

// ==============================================================================================================================================================================================

/* ------------ Watch Dog Timer ---------------- */

#define WDT_TIMEOUT 300          // 5 Minute

#define SERIAL_SIZE_RX  10000    // used in Serial.setRxBufferSize

// ==============================================================================================================================================================================================

struct My_Object {
  char ssid[25];
  char pass[25];
  char cstr[150];
};

// ==============================================================================================================================================================================================

My_Object customVarr;

// ==============================================================================================================================================================================================

static bool isHubConnect = false;

// ==============================================================================================================================================================================================

unsigned long timer = 15000; // 15 Seconds

// ==============================================================================================================================================================================================

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ==============================================================================================================================================================================================

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {

        /* ----- Make object from struct to carry credentials ----- */
        My_Object Credentials;
        char WiFi_SSID[25] = "";
        char WiFi_PASS[25] = "";
        char Conec_STR[150] = "";

        /* ----- Extract Connection String ----- */
        if ( value[0] == '@' ) {

          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            Serial.print(value[i]);
          }

          for (int i = 1; i < value.length(); i++) {
            Conec_STR[i - 1] = value[i];
          }

          Serial.print("Connection String : ");
          Serial.println(Conec_STR);
          memcpy(Credentials.cstr, Conec_STR, sizeof(Credentials.cstr));

        } else {
          int x;
          Serial.print("Entered value: ");
          for (int i = 0; i < value.length(); i++) {
            if ( value[i] == ',' ) {
              x = i;
            }
            Serial.print(value[i]);
          }
          Serial.println("");
          Serial.print("Comma is at index : ");
          Serial.println(x);

          /* ----- Extract WiFi SSID ----- */

          for (int i = 0; i < x; i++) {
            WiFi_SSID[i] = value[i];
          }
          Serial.print("WiFi_Username : ");
          Serial.println(WiFi_SSID);
          memcpy(Credentials.ssid, WiFi_SSID, sizeof(Credentials.ssid));

          /* ----- Extract WiFi Password ----- */

          for (int i = x + 1; i < value.length(); i++) {
            WiFi_PASS[i - (x + 1)] = value[i];
          }
          Serial.print("WiFi_Password : ");
          Serial.println(WiFi_PASS);
          memcpy(Credentials.pass, WiFi_PASS, sizeof(Credentials.pass));

          /* ----- Keep the Connection String Same ----- */

          EEPROM.get(0, customVarr);
          Serial.println(customVarr.cstr);
          memcpy(Credentials.cstr, customVarr.cstr, sizeof(Credentials.cstr));
        }

        /* ----- Store WiFi Credentials to EEPROM Address 0 ----- */
        EEPROM.put(0, Credentials);
        EEPROM.commit();
      }
    }

};

/* =============================================================================================================================================================================== */

void setup() {
  
  Serial.begin(500000);
  Serial.setRxBufferSize(SERIAL_SIZE_RX);
  Serial1.begin(500000);
  Serial1.setRxBufferSize(SERIAL_SIZE_RX);
  EEPROM.begin(500);
  delay(5000);

  Serial.println("Configurating Watch Dog Timer ....");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  pinMode(21, OUTPUT);  // BLE Switch
  pinMode(14, OUTPUT);  // Red WiFi LED
  pinMode(15, OUTPUT);  // Green RGB LED
  pinMode(32, OUTPUT);  // Red RGB LED
  pinMode(33, OUTPUT);  // Blue RGB LED

  /* ----- Retrieve WiFi Credentials from EEPROM ----- */
  EEPROM.get(0, customVarr);
  Serial.print("WiFi_Username : "); Serial.println(customVarr.ssid);
  Serial.print("WiFi_Password : "); Serial.println(customVarr.pass);
  Serial.print("Conection Str : "); Serial.println(customVarr.cstr);

  /* ----- Turn ON the bluetooth ----- */
  if (digitalRead(21) == HIGH) {
    BLEDevice::init("ProCheck WiFi Shield");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
  }

  /* ----- Connect to WiFi through saved credentials ----- */
  Connect_To_WiFi(customVarr.ssid, customVarr.pass);

}

/* =============================================================================================================================================================================== */

void loop() {

  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  /*-----------------------------------------------------------    Send Empty Packet to Azure IoT Hub and Fetch Data signal to PLC   -----------------------------------------------------------------*/
  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

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

      /* Reconnect to wifi if there is a failure in send request */
      WiFi.disconnect();
      Connect_To_WiFi(customVarr.ssid, customVarr.pass);
    }

    /* Increment Empty Packet Sending Time by 15 Seconds */
    timer = millis() + 15000UL;
    Serial1.write("3");
    Serial.println("");
    Serial.println("Sent fetch data signal to PLC");
    Serial.println("");
  }

  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------------  Read Serial data from PLC  -------------------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

  /* Check for Serial input pin "Rx" */
  if (Serial1.available() > 0) {

    /* Create an Array as a buffer to store incoming serial data */
    char bfr[5001];
    memset(bfr, 0, 5001);

    /* readBytesUntil(Terminator, data, dataLength) */
    Serial1.readBytesUntil('A', bfr, 5000);
    Serial.println(bfr);

    /* Blink RGB Red on data recieve from Rx */
    digitalWrite(32, 1);
    delay(300);
    digitalWrite(32, 0);

    if (isHubConnect && Esp32MQTTClient_SendEvent(bfr)) {
      Serial.println("Successfully sent to Azure IoT Hub");

      /* Blink RGB Green Led on successfully data send through mqtt */
      digitalWrite(15, 1);
      delay(300);
      digitalWrite(15, 0);

      /* ----- Data successfully sent signal to PLC ----- */
      Serial1.write("4");
      Serial.println("");
      Serial.println("Sent confirmation signal to PLC");
      Serial.println("");

    } else {

      Serial.println("Failure occur in sending to Azure IoT Hub");

      /* Blink RGB Blue Led on failure in data sending through mqtt */
      digitalWrite(33, 1);
      delay(300);
      digitalWrite(33, 0);
      delay(50);

      /* Reconnect to wifi if there is a failure in send request */
      WiFi.disconnect();
      Connect_To_WiFi(customVarr.ssid, customVarr.pass);
    }
  }

  delay(1000);
  esp_task_wdt_reset();
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------    Connect to WiFi -> Ping to Google -> Connect to Azure IoT Hub   -------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

void Connect_To_WiFi(const char * ssid, const char * pwd) {

  int ledState = 0;
  unsigned long WiFi_Time_Check = millis();

  //define hostname
  String hostname = "Procheck WiFi Shield";
  WiFi.setHostname(hostname.c_str()); 
  Serial.print("Connecting to WiFi network: " + String(ssid));
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);
    ledState = ( ledState + 1 ) % 2;     // Flip led State
    delay(500);
    Serial.print(".");
    if ( ( millis() - WiFi_Time_Check ) > 900000 ) { ESP.restart(); }
  }

  digitalWrite(14, 0);
  Serial.println("");

  /* Printing Network Credentials */
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

  /* Making Ping to Google to Check Internet Connection */
  bool Ping_Success = Ping.ping("www.google.com", 3);

  if (!Ping_Success) {
    Serial.println("Failed to Ping www.google.com");
    WiFi.disconnect();
    Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  } else {
    Serial.println();
    Serial.println("Ping successful to www.google.com");
    Serial.println();
  }

  /* ----- Connect to Azure IoT Hub through Connection String ----- */
  if (!Esp32MQTTClient_Init((const uint8_t*)customVarr.cstr)) {
    isHubConnect = false;
    Serial.println("Initializing IoT hub failed.");
    WiFi.disconnect();
    Connect_To_WiFi(customVarr.ssid, customVarr.pass);
  }
  isHubConnect = true;
  Serial.println("Connection Established with Azure IoT Hub");
}
