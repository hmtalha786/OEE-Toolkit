extern "C" {
#include "crypto/base64.h"
}
 
void setup() {
  Serial.begin(9600);
 
  char * toDecode = "eyJuYW1lIjoiU2FtIiwgImFnZSI6MzIsICJzdGF0dXMiOjF9\n";
  size_t outputLength;
 
  unsigned char * decoded = base64_decode((const unsigned char *)toDecode, strlen(toDecode), &outputLength);
 
  Serial.print("Length of decoded message: ");
  Serial.println(outputLength);
 
  Serial.printf("%.*s", outputLength, decoded);
  free(decoded);
}
 
void loop() {}
