//An example of Simplecomm Slave communication:
#include <RS485.h>
#include <SimpleComm.h>

// Create SimplePacket for sending and receiving data
SimplePacket request;
SimplePacket response;
// Define slave address to communicate with
uint8_t slaveAddress = 1;
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600L);
  // Start RS485
  RS485.begin(19200L);
  RS485.setTimeout(20);
  // Start SimpleComm
  SimpleComm.begin(slaveAddress);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Get requests
  if (SimpleComm.receive(RS485, request)) {
    int value = request.getInt();
    Serial.print("Received value: ");
    Serial.println(value);
if ( value==5){
  digitalWrite(Q0_0,HIGH);
  delay(5000);
  digitalWrite(Q0_0,LOW);
  
}
    // Process value
    //value++;
    // Send response to the request packet source
    response.setData(value);
    if (SimpleComm.send(RS485, response, request.getSource())) {
      Serial.print("Sent value: ");
      Serial.println(value);
    }
  }
}
