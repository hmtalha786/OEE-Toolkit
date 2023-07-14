#include <ArduinoJson.h>

//====================================================================================================================================================================

char Ref_Array[90] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                      'z', 'y', 'x', 'w', 'v', 'u', 't', 's', 'r', 'q', 'p', 'o', 'n', 'm', 'l', 'k', 'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                      '{', '}', '[', ']', ' ', ',', '"', ':', '@', '?'
                     };

char Enc_Array[90] = {'(', ')', '_', ';', '$', ':', '-', '+', '*', '/',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                      'z', 'y', 'x', 'w', 'v', 'u', 't', 's', 'r', 'q', 'p', 'o', 'n', 'm', 'l', 'k', 'j', 'i', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a',
                      '9', '8', '7', '6', '5', '4', '3', '2', '1', '0'
                     };


//====================================================================================================================================================================

void setup() {
  Serial.begin(9600);
}

//====================================================================================================================================================================

void loop() {

  if (Serial.available() > 0) {
    String Msg;
    Serial.println("-------------------------");
    Serial.print("Encrypted Value : ");
    Msg += Serial.readString();
    Serial.println(Msg);
    Decrypt(Msg);
  }

}

//====================================================================================================================================================================

void Decrypt(String Str) {   

  int MVL = Str.length();                      // Message Value Length
  int RAL = sizeof(Ref_Array);                 // Refference Array Length

  String Decrypted_Msg = "";                   // Clear the decrypted message string

  for (int i = 0; i < MVL; i++) {
    for (int j = 0; j < RAL; j++) {
      if (Str[i] == Enc_Array[j]) {
        Decrypted_Msg += Ref_Array[j];
        break;                                // Break out of the inner loop once a match is found
      }
    }
  }

  Serial.print("Decrypted Message : ");
  Serial.println(Decrypted_Msg);
  Serial.println("");

}
