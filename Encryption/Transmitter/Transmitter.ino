#include <ArduinoJson.h>

String Encrypted_Data[500];

String Msg;
String Decrypted_Msg;

//====================================================================================================================================================================

char Ref_Array[60] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                      '{', '}', '[', ']', ' ', ',', '"', ':', '@', '?'
                     };

char Enc_Array[60] = {'(', ')', '_', ';', '$', ':', '-', '+', '*', '/',
                      'z', 'y', 'x', 'w', 'v', 'u', 't', 's', 'r', 'q', 'p', 'o', 'n', 'm', 'l', 'k', 'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a',
                      '9', '8', '7', '6', '5', '4', '3', '2', '1', '0'
                     };

//====================================================================================================================================================================

void setup() {
  Serial.begin(9600);
}

//====================================================================================================================================================================

void loop() {

  StaticJsonDocument<500> doc;

  doc["PTS"] = millis();
  doc["SR1"] = 22;
  doc["SR2"] = 22;
  doc["SR3"] = 22;
  doc["SR4"] = 22;
  doc["SS1"] = 0;
  doc["SS2"] = 0;
  doc["SS3"] = 0;
  doc["SS4"] = 0;

  // Serialize the JSON object to a string
  serializeJson(doc, Msg);
  Serial.print("Sample Data Packet : ");
  Serial.println(Msg);

  delay(5000);

  Encrypt(Msg);
//  Decrypt();
  Msg = "";
  delay(10000);
}

//====================================================================================================================================================================

void Encrypt(String Str) {

  int MVL = Str.length();                      // Message Value Length
  int RAL = sizeof(Ref_Array);                 // Refference Array Length

  Serial.print("Encrypted Value : ");

  for (int i = 0; i < MVL; i++) {
    for (int j = 0; j < RAL; j++) {
      if (Msg[i] == Ref_Array[j]) {
        Encrypted_Data[i] = Enc_Array[j];
      }
    }
    Serial.print(Encrypted_Data[i]);
    delay(10);
  }

  Serial.println("");

}

//====================================================================================================================================================================
//
//void Decrypt() {
//
//  Decrypted_Msg = ""; // Clear the decrypted message string
//
//  for (int i = 0; i < MVL; i++) {
//    for (int j = 0; j < RAL; j++) {
//      if (String(Encrypted_Data[i]) == String(Enc_Array[j])) {
//        Decrypted_Msg += String(Ref_Array[j]);
//        break; // Break out of the inner loop once a match is found
//      }
//    }
//  }
//
//  Serial.print("Decrypted Message : ");
//  Serial.println(Decrypted_Msg);
//  Serial.println("");
//}
