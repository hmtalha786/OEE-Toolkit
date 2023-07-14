#include <RS485.h>
#include <SimpleComm.h>
// Create SimplePacket for sending and receiving data
SimplePacket packet;
// Define master address
uint8_t masterAddress = 0;
// Define slave address to communicate with
uint8_t slaveAddress = 1;
// Value to send as packet data
int value = 5;
////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600L);
  // Start RS485
  RS485.begin(19200L);
  RS485.setTimeout(20);
  // Start SimpleComm
  SimpleComm.begin(masterAddress);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  static unsigned long lastSent = millis();
  // Send packet periodically: once per second
  if (millis() - lastSent >= 10000) {
    // Set request packet data
    packet.setData(value);
    // Send request to slave
    if (SimpleComm.send(RS485, packet, slaveAddress)) {
      lastSent = millis();
      Serial.print("Sent value: ");
      Serial.println(value);
    }
  }
  // Get responses
  if (SimpleComm.receive(RS485, packet)) {
    // Update value from the response
    value = packet.getInt();
    Serial.print("Received value: ");
    Serial.println(value);
  }
}
