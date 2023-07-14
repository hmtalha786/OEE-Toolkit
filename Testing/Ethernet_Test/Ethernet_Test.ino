#include <Ethernet.h>

#define Server "www.google.com"

const uint16_t port = 80;
uint8_t mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01};

EthernetClient client;

////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(500000UL);
  Serial.println("Ethernet Connection Started");
  test();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  if (!client.connected()) { Serial.println("Disconnected"); test(); }
  delay(1000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void test() {
  //if you need a DHCP IP use this configuration:
  Ethernet.begin(mac);

  switch (Ethernet.hardwareStatus()) {
    case EthernetW5100:
      Serial.println("W5100 found");
      break;

    case EthernetW5200:
      Serial.println("W5200 found");
      break;

    case EthernetW5500:
      Serial.println("W5500 found");
      break;

    default:
      Serial.println("Unknown hardware");
      break;
  }

  uint8_t MAC[6];
  Ethernet.MACAddress(MAC);
  for (int i = 0; i < 6; ++i) {
    if (i > 0) {
      Serial.print(':');
    }
    Serial.print(MAC[i], HEX);
  }
  Serial.println();

  //use this block with DHCP IP:
  Serial.println(Ethernet.localIP());
  if (Ethernet.localIP() == IPAddress({0, 0, 0, 0})) {
    Serial.println("Local IP FAIL");
  } else {
    Serial.println("Local IP OK");
    if (client.connect(Server, port)) {
      Serial.println("Server connection OK");
    } else {
      Serial.println("Server connection FAIL");
    }
  }
}
