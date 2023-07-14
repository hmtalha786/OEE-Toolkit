/********************************
 * Libraries included
 *******************************/
#include <SPI.h>
#include <Ethernet2.h>

/********************************
 * Constants and objects
 *******************************/
 
/* Enter a MAC address for your controller below.
   Newer Ethernet shields have a MAC address printed on a sticker on the shield */
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/* Set the static IP address to use if the DHCP fails to assign */
IPAddress ip(192, 168, 0, 177);

/* Initialize the Ethernet client library
   with the IP address and port of the server
   that you want to connect to (port 80 is default for HTTP) */
EthernetClient client;

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

//#define WIZ_RESET 9

#if defined(ESP8266)
  // default for ESPressif
  #define WIZ_CS 15
#elif defined(ESP32)
  #define WIZ_CS 33
#elif defined(ARDUINO_STM32_FEATHER)
  // default for WICED
  #define WIZ_CS PB4
#elif defined(TEENSYDUINO)
  #define WIZ_CS 10
#elif defined(ARDUINO_FEATHER52)
  #define WIZ_CS 11
#else   // default for 328p, 32u4 and m0
  #define WIZ_CS 10
#endif


const char* TOKEN = "...."; // Put here your TOKEN
const char* DEVICE_LABEL = "feather-ethernet"; // Your device label
const char* VARIABLE_LABEL = "temperature"; // Your variable label
const char* HTTPSERVER = "things.ubidots.com";   
const char* USER_AGENT = "ESP8266";
const char* VERSION = "1.0";

/********************************
 * Auxiliar Functions
 *******************************/
/**
 * Gets the length of the body
 * @arg variable the body of type char
 * @return dataLen the length of the variable
 */
int dataLen(char* variable) {
  uint8_t dataLen = 0;
  for (int i = 0; i <= 250; i++) {
    if (variable[i] != '\0') {
      dataLen++;
    } else {
      break;
    }
  }
  return dataLen;
}

/********************************
 * Main Functions
 *******************************/

void setup() {
#if defined(WIZ_RESET)
  pinMode(WIZ_RESET, OUTPUT);
  digitalWrite(WIZ_RESET, HIGH);
  delay(100);
  digitalWrite(WIZ_RESET, LOW);
  delay(100);
  digitalWrite(WIZ_RESET, HIGH);
#endif

#if !defined(ESP8266) 
  /* wait for serial port to connect */
  while (!Serial); 
#endif

  /* Open serial communications and wait for port to open */
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nSetting up...");

  Ethernet.init(WIZ_CS);
  
  /* give the ethernet module time to boot up */
  delay(1000);

  /* start the Ethernet connection */
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    /* no point in carrying on, so do nothing forevermore:
       try to congifure using IP address instead of DHCP */
    Ethernet.begin(mac, ip);
  }
  
  /* print the Ethernet board/shield's IP address */
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  /* if there's incoming data from the net connection.
     send it out the serial port.  This is for debugging
     purposes only */
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  /* if ten seconds have passed since your last connection,
     then connect again and send data */
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

/* this method makes a HTTP connection to the server */
void httpRequest() {
  char* body = (char *) malloc(sizeof(char) * 150);
  char* data = (char *) malloc(sizeof(char) * 300);
  /* Space to store values to send */
  char str_val[10];
  
  /* close any connection before send a new request.
     This will free the socket on the WiFi shield */
  client.stop();

  /* Read the sensor from the analog input*/
  float sensor_value = analogRead(A0);

  /*---- Transforms the values of the sensors to char type -----*/
  /* 4 is mininum width, 2 is precision; float value is copied onto str_val*/
  dtostrf(sensor_value, 4, 2, str_val);

  /* Builds the body to be send into the request*/ 
  sprintf(body, "{\"%s\":%s}", VARIABLE_LABEL, str_val);

  /* Builds the HTTP request to be POST */
  sprintf(data, "POST /api/v1.6/devices/%s", DEVICE_LABEL);
  sprintf(data, "%s HTTP/1.1\r\n", data);
  sprintf(data, "%sHost: things.ubidots.com\r\n", data);
  sprintf(data, "%sUser-Agent: %s/%s\r\n", data, USER_AGENT, VERSION);
  sprintf(data, "%sX-Auth-Token: %s\r\n", data, TOKEN);
  sprintf(data, "%sConnection: close\r\n", data);
  sprintf(data, "%sContent-Type: application/json\r\n", data);
  sprintf(data, "%sContent-Length: %d\r\n\r\n", data, dataLen(body));
  sprintf(data, "%s%s\r\n\r\n", data, body);
  
  /* if there's a successful connection */
  if (client.connect(HTTPSERVER, 80)) {
    Serial.println("connecting...");
    /* send the HTTP POST request */
    client.print(data);
    /* note the time that the connection was made */
    lastConnectionTime = millis();
  }
  else {
    /* if you couldn't make a connection */
    Serial.println("connection failed");
  }

  /* Free memory */
  free(data);
  free(body);
} 
