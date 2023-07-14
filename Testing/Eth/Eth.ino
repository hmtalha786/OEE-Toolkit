#include <Ethernet.h>
#include <SPI.h>

// Conf. mac 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Server to Post
// char server[] = "http://abc.requestcatcher.com/test";

char server[] = "https://api.datacake.co/integrations/api/";

// Starting Ethernet client
EthernetClient client;

// =============== Connecting to internet =============== //

void setup() {
  // Open serial communications and wait for port to open:
  // wait for serial port to connect. Needed for native USB port only
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }

    // Connecting to internet
  if (Ethernet.begin (mac) == 0) {
    Serial.println("Can’t connect via DHCP");
  }

  // Give the Ethernet shield a second to initialize
  delay(1000);
  // Printing the IP Adress
  Serial.print ("IP Address: ");
  Serial.println(Ethernet.localIP());

}

/////============= Sending Post request ============= ////

void loop() {

  Serial.println(" - Post request in process - ");

  if (client.connect(server, 80)) { 
        Serial.print(" Sending Post request ");
        client.println("POST /integrations/api/1a679601-42aa-44db-a966-a4fb7f91e3f0/ HTTP/1.1"); 
        client.println("Host: api.datacake.co/"); 
        client.println("Content-Type: application/x-www-form-urlencoded"); 
        // client.println("Content-Length: "); 
        client.println(); 
  } 
  else {
    Serial.println("Can’t reach the server");
  }
  // Wait 10 secs
  delay(10000); 

}