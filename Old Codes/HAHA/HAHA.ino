#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

byte mac[6];

int address = 0;
byte valuee;
const int BUTTON_PIN = 0;
const int LED_PIN = 14;
float wifiStrength;
unsigned long timechck = 0;

char linkk[100] = "https://oee.procheck.pk/api/v1/data-entry/";

int flag = 0;
int x = 0;
struct MyObject {
  char ssid[25];
  char pass[25];
  char link[30];
};

MyObject customVarr;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
        {
          Serial.print(value[i]);
          if (value[i] == ',')
          {
            x = i;
          }
        }

        Serial.println(x);

        int y = 0;

        MyObject cred;
        char tap[25] = "";
        char rap[25] = "";
        char lin[30] = "";

        if (value[0] == ':')
        {
          for (int b = 0; b < value.length(); b++)
          {
            lin[b] = value[b + 1];
          }
          y = 1;
        }
        else
        {
          for (int j = 0; j < x; j++)
          {
            tap[j] = value[j];
          }
          for (int k = x + 1; k < value.length(); k++)
          {
            rap[k - (x + 1)] = value[k];
          }
          y = 2;
        }

        Serial.println(tap);
        Serial.println(rap);
        Serial.println(lin);
        if (y == 2)
        {
          memcpy(cred.ssid, tap, sizeof(cred.ssid));
          memcpy(cred.pass, rap, sizeof(cred.pass));

          EEPROM.get(0, customVarr);

          memcpy(cred.link, customVarr.link, sizeof(cred.link));

          EEPROM.put(0, cred);
          EEPROM.commit();
          //          Serial.println("id password");
        }
        else if (y == 1)
        {

          EEPROM.get(0, customVarr);
          memcpy(cred.ssid, customVarr.ssid, sizeof(cred.ssid));
          memcpy(cred.pass, customVarr.pass, sizeof(cred.pass));
          memcpy(cred.link, lin, sizeof(cred.link));
          EEPROM.put(0, cred);
          EEPROM.commit();
          //          Serial.println("linkkkkkkkkkkk");
        }

        Serial.println("*********");
      }


    }
};

void setup() {
 // Serial.begin(500000);
  Serial.begin(115200);
  EEPROM.begin(85);
  delay(5000);
  //Serial1.begin(500000);
  Serial1.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(21, OUTPUT);


  EEPROM.get(0, customVarr);
  Serial.println(customVarr.ssid);
  Serial.println(customVarr.pass);
  Serial.println(customVarr.link);

  strcat(linkk, customVarr.link);
  Serial.println(linkk);

  if (digitalRead(21) == HIGH)
  {
    Serial.println("BOSS");
    BLEDevice::init("PROCHECK WIFI SHIELD");
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                           CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );

    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World");
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
  }
  //  const char * networkName = customVarr.ssid;
  //  const char * networkPswd = customVarr.pass;
  //  connectToWiFi(networkName, networkPswd);
  connectToWiFi(customVarr.ssid, customVarr.pass);
}

void loop() {


  while (WiFi.status() == WL_CONNECTED)
  {
    if (Serial1.available() > 0) {
      char bfr[501];
      //delay(500);
      Serial.println("------------------");
      memset(bfr, 0, 501);
      //     Serial.println("1");
      Serial1.readBytesUntil('A', bfr, 500);
      //     Serial.println("2");

      digitalWrite(32, 1);
      delay(300);
      digitalWrite(32, 0);

      HTTPClient http;    //Declare object of class HTTPClient
      http.begin(linkk);
      //     http.begin("https://oee.procheck.pk/api/v1/data-entry/line-1-(norden-3002)_efvd1");

      http.addHeader("Content-Type", "application/json");  //Specify content-type header

      int httpCode = http.POST(bfr);   //Send the request
      Serial.println(bfr);
      String payload = http.getString();                                        //Get the response payload
      Serial.print("httpCode = ");
      Serial.println(httpCode);   //Print HTTP return code
      Serial.print("payload = ");
      Serial.println(payload);
      http.end();
      if (httpCode == 200)
      {
        digitalWrite(15, 1);
        delay(300);
        digitalWrite(15, 0);
      }
      if (httpCode == -1)
      {
        digitalWrite(33, 1);
        delay(300);
        digitalWrite(33, 0);
      }
//       Serial.println("hm123");
      unsigned long smth = millis();
      unsigned long smth1 = millis();
      while (Serial1.available() || (smth1 < 2000))
      {
        smth1 = millis() - smth;
        Serial1.read();
      }
//         Serial.println("hm456");
    }
    delay(100);
//    Serial.println("hm");
  }
  connectToWiFi(customVarr.ssid, customVarr.pass);

}

void connectToWiFi(const char * ssid, const char * pwd)
{
  int ledState = 0;

  printLine();
  Serial.println("Connecting to WiFi network: " + String(ssid));
  timechck = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink LED while we're connecting:
    digitalWrite(LED_PIN, ledState);
    ledState = (ledState + 1) % 2; // Flip ledState
    delay(500);
    Serial.print(".");
    if (millis() - timechck > 600000)
    {
      //system_reboot();
      ESP.restart();
    }
  }
  digitalWrite(LED_PIN, 0);
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void printLine()
{
  Serial.println();
  for (int i = 0; i < 30; i++)
    Serial.print("-");
  Serial.println();
}
