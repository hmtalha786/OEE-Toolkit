#include <WiFi.h>
#include <EEPROM.h>
#include <stdio.h>
#include <string.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <HTTPClient.h>

// =================================================================================================================================================================================

char linkk[100] = "https://procheck-oee-uat.azure-devices.net/devices/";
char linkik[60] = "/messages/events?api-version=2018-06-30";

char SASFirst[180] = "SharedAccessSignature sr=procheck-oee-uat.azure-devices.net%2Fdevices%2F";
char SASMid[6] = "&sig=";

// =================================================================================================================================================================================

unsigned long WiFi_Time_Check = 0;
int Restart_Flag = 0;
int x = 0;

// =================================================================================================================================================================================

HTTPClient https;

const char* root_ca = \
                      "-----BEGIN CERTIFICATE-----\r\n" \
                      "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n" \
                      "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n" \
                      "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n" \
                      "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n" \
                      "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n" \
                      "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n" \
                      "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n" \
                      "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n" \
                      "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n" \
                      "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n" \
                      "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n" \
                      "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n" \
                      "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n" \
                      "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n" \
                      "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n" \
                      "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n" \
                      "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n" \
                      "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n" \
                      "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n" \
                      "-----END CERTIFICATE-----\r\n";

struct MyObject {
  char ssid[25];
  char pass[25];
  char link[25];
  char SAK[100]; //to beLOOKED ATTTTT
  char pKey[100];
  char flgRst[1];
};

MyObject customVarr;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// =================================================================================================================================================================================

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
        char lin[25] = "";
        char linSAK[100] = "";
        char linpKey[100] = "";

        if (value[0] == ':') // For Device ID
        {
          for (int b = 0; b < value.length(); b++)
          {
            lin[b] = value[b + 1];
          }
          y = 1;
        }
        else if (value[0] == '>') //For SAS token unique part
        {
          for (int b = 0; b < value.length(); b++)
          {
            linSAK[b] = value[b + 1];
          }
          y = 3;
        }
        else if (value[0] == '=')//For Primary Key
        {
          for (int b = 0; b < value.length(); b++)
          {
            linpKey[b] = value[b + 1];
          }
          y = 4;
        }
        else //For WiFi credentials
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
        Serial.println(linSAK);
        if (y == 2)
        {
          memcpy(cred.ssid, tap, sizeof(cred.ssid));
          memcpy(cred.pass, rap, sizeof(cred.pass));

          EEPROM.get(0, customVarr);

          memcpy(cred.link, customVarr.link, sizeof(cred.link));
          memcpy(cred.SAK, customVarr.SAK, sizeof(cred.SAK));
          memcpy(cred.flgRst, customVarr.flgRst, sizeof(cred.flgRst));
          memcpy(cred.pKey, customVarr.pKey, sizeof(cred.pKey));

          EEPROM.put(0, cred);
          EEPROM.commit();
        }
        else if (y == 1)
        {

          EEPROM.get(0, customVarr);
          memcpy(cred.ssid, customVarr.ssid, sizeof(cred.ssid));
          memcpy(cred.pass, customVarr.pass, sizeof(cred.pass));

          memcpy(cred.link, lin, sizeof(cred.link));

          memcpy(cred.SAK, customVarr.SAK, sizeof(cred.SAK));
          memcpy(cred.flgRst, customVarr.flgRst, sizeof(cred.flgRst));
          memcpy(cred.pKey, customVarr.pKey, sizeof(cred.pKey));
          EEPROM.put(0, cred);
          EEPROM.commit();
          Serial.println("Comiteeddddddddddddddddddd");
        }
        else if (y == 3)
        {

          EEPROM.get(0, customVarr);
          memcpy(cred.ssid, customVarr.ssid, sizeof(cred.ssid));
          memcpy(cred.pass, customVarr.pass, sizeof(cred.pass));
          memcpy(cred.link, customVarr.link, sizeof(cred.link));

          memcpy(cred.SAK, linSAK, sizeof(cred.SAK));
          memcpy(cred.flgRst, customVarr.flgRst, sizeof(cred.flgRst));
          memcpy(cred.pKey, customVarr.pKey, sizeof(cred.pKey));
          EEPROM.put(0, cred);
          EEPROM.commit();
          //          Serial.println("linkkkkkkkkkkk");
        }
        else if (y == 4)
        {
          EEPROM.get(0, customVarr);
          memcpy(cred.ssid, customVarr.ssid, sizeof(cred.ssid));
          memcpy(cred.pass, customVarr.pass, sizeof(cred.pass));
          memcpy(cred.link, customVarr.link, sizeof(cred.link));
          memcpy(cred.SAK, customVarr.SAK, sizeof(cred.SAK));

          memcpy(cred.pKey, linpKey, sizeof(cred.pKey));

          memcpy(cred.flgRst, customVarr.flgRst, sizeof(cred.flgRst));
          EEPROM.put(0, cred);
          EEPROM.commit();
        }

        Serial.println("*********");
      }


    }
};

// =================================================================================================================================================================================

void setup() {

  Serial.begin(500000);
  EEPROM.begin(300);
  delay(5000);
  Serial1.begin(500000);
  pinMode(14, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(21, OUTPUT);

  EEPROM.get(0, customVarr);
  Serial.println("************************EEPROM-START*****************************");
  Serial.print("SSID: ");         Serial.println(customVarr.ssid);
  Serial.print("PASS: ");         Serial.println(customVarr.pass);
  Serial.print("Device ID: ");    Serial.println(customVarr.link);
  Serial.print("SAS Key: ");      Serial.println(customVarr.SAK);
  Serial.print("Primary Key: ");  Serial.println(customVarr.pKey);
  Serial.print("Get SAS Flag: "); Serial.println(customVarr.flgRst);
  Serial.println("************************EEPROM-END*******************************");

  // Making Http endpoint using saved line ID
  strcat(linkk, customVarr.link);
  strcat(linkk, linkik);
  Serial.println(linkk);

  // Making SAS toke using saved line Id and SAS key
  strcat(SASFirst, customVarr.link);
  strcat(SASFirst, SASMid);
  strcat(SASFirst, customVarr.SAK);
  Serial.println(SASFirst);

  if (digitalRead(21) == HIGH)
  {
    Serial.println("BOSS");
    BLEDevice::init("PROCHECK WIFI SHIELD");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
  }

  connectToWiFi(customVarr.ssid, customVarr.pass);

  Serial.println("=======================================Gonna check for customVarr.SAK is 0 or 1 =======================================");
  String chkok = customVarr.flgRst;
  Serial.println(chkok);
  if (chkok == "1")
  {
    Serial.println("=====================================In because 1 =======================================");
    HTTPClient httpSAS;    // Declare object of class HTTPClient
    httpSAS.begin("https://procheck-oee-uat.azurewebsites.net/api/GetSasToken?code=GfN9tAJp48LljMxjO_cBfL6iSr1H9bR986vFkLz0epmqAzFuar3L0Q=="); // Special HTTP Endpoint for SAS token Generation
    httpSAS.addHeader("Content-Type", "application/json");  // Specify content-type header

    String ID = customVarr.link;
    String PKEY = customVarr.pKey;
    String tyt = "{\"id\":\"" + ID + "\", " + "\"key\":\"" + PKEY + "\"}";
    Serial.println(tyt);
    int httpCodeSAS = httpSAS.POST(tyt);         // Send the request
    Serial.println(tyt);
    Serial.println("===========================================================================================================");
    String payloadSAS = httpSAS.getString();     // Get the response payload
    Serial.print("httpCode = ");
    Serial.println(httpCodeSAS);                 // Print HTTP return code
    Serial.print("payload = ");
    Serial.println(payloadSAS);
    httpSAS.end();

    if (httpCodeSAS == 200)
    {
      MyObject ForSAS;
      //Saving recieved SAS token to EEPROM
      memcpy(ForSAS.ssid, customVarr.ssid, sizeof(ForSAS.ssid));
      memcpy(ForSAS.pass, customVarr.pass, sizeof(ForSAS.pass));
      memcpy(ForSAS.link, customVarr.link, sizeof(ForSAS.link));

      char tempForSAS [100];
      payloadSAS.toCharArray(tempForSAS, 100);

      memcpy(ForSAS.SAK, tempForSAS, sizeof(ForSAS.SAK));
      memcpy(ForSAS.flgRst, "0", sizeof(ForSAS.flgRst));
      memcpy(ForSAS.pKey, customVarr.pKey, sizeof(ForSAS.pKey));
      EEPROM.put(0, ForSAS);
      EEPROM.commit();
    }
    delay(60000);
    ESP.restart();
  }

  https.setReuse(true);
}

// =================================================================================================================================================================================

void loop() {

  while (WiFi.status() == WL_CONNECTED)
  {
    if (Serial1.available() > 0) {
      char bfr[501];
      Serial.println("------------------");
      memset(bfr, 0, 501);
      Serial1.readBytesUntil('A', bfr, 500);

      digitalWrite(32, 1);
      delay(300);
      digitalWrite(32, 0);

      // Declare object of class HTTPClient
      https.begin(linkk);
      https.addHeader("Authorization", SASFirst);
      https.addHeader("Content-Type", "application/json");    // Specify content-type header

      int httpCode = https.POST(bfr);                         // Send the request
      Serial.println(bfr);
      String payload = https.getString();                     // Get the response payload
      Serial.print("httpCode = ");
      Serial.println(httpCode);                               // Print HTTP return code
      Serial.print("payload = ");
      Serial.println(payload);
      https.end();


      if (httpCode == 200 || httpCode == 204) {
        digitalWrite(15, 1);
        delay(300);
        digitalWrite(15, 0);
      }

      if (httpCode == -1)
      {
        digitalWrite(33, 1);
        delay(300);
        digitalWrite(33, 0);
        Restart_Flag++;
        if (Restart_Flag > 50)
        {
          ESP.restart();
        }
      }

      if (httpCode == 400 || httpCode == 401 ) // Getting SAS Token
      {
        Serial.println("Gona do tht thing");
        MyObject ForSAS;
        //Saving recieved SAS token to EEPROM
        memcpy(ForSAS.ssid, customVarr.ssid, sizeof(ForSAS.ssid));
        memcpy(ForSAS.pass, customVarr.pass, sizeof(ForSAS.pass));
        memcpy(ForSAS.link, customVarr.link, sizeof(ForSAS.link));
        memcpy(ForSAS.SAK, customVarr.SAK, sizeof(ForSAS.SAK));
        memcpy(ForSAS.flgRst, "1", sizeof(ForSAS.flgRst));
        memcpy(ForSAS.pKey, customVarr.pKey, sizeof(ForSAS.pKey));
        EEPROM.put(0, ForSAS);
        EEPROM.commit();

        ESP.restart();
      }

      while (Serial1.available()) {
        Serial1.read();
      }

    }
  }
  connectToWiFi(customVarr.ssid, customVarr.pass);
}

// =================================================================================================================================================================================

void connectToWiFi(const char * ssid, const char * pwd) {
  int ledState = 0;
  Serial.println("-------------------------------------------------------------------------");
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi_Time_Check = millis();
  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, ledState);
    ledState = (ledState + 1) % 2; // Flip ledState
    delay(500);
    Serial.print(".");
    if (millis() - WiFi_Time_Check > 60000) {
      ESP.restart();
    }
  }

  digitalWrite(14, 1);
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// =================================================================================================================================================================================

void printLine() {
  Serial.println();
  for (int i = 0; i < 30; i++) {
    Serial.print("-");
    Serial.println();
  }
}

// =================================================================================================================================================================================

int getStrength() {
  long rssi = 0;
  long averageRSSI = 0;
  for (int i = 0; i < 7; i++) {
    rssi += WiFi.RSSI();
    delay(20);
  }
  averageRSSI = rssi / 7;
  return averageRSSI;
}
