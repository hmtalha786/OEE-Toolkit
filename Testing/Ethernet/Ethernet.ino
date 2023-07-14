#include <ArduinoHttpClient.h>
#include <Ethernet.h>

const char* ssid = "Procheck";
const char* pass = "Procheck@123";
const char* url =  "https://procheck-prod.azure-devices.net/devices/testingnow_avi63/messages/events?api-version=2020-03-13";
const char* sas =  "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2Ftestingnow_avi63&sig=%2Bwgm%2BuRpHnN5jn8px4916zLWMUUxcWjHgmHJcXGH8bQ%3D&se=1685168670";
const char* body = "{\"device\":\"357073299541579\",\"temp\":\"24.6\",\"hum\":\"41.25\",\"bat\":\"41.54\"}";

const char server[150] = "https://procheck-prod.azure-devices.net/devices/testingnow_avi63/";
int port = 80;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress server(192, 168, 1, 209);
//int port = 1880;

EthernetClient ether;
HttpClient client = HttpClient(ether, server, port);
String response;


void setup() {
  Serial.begin(500000);
  // Ethernet.begin(mac,ip,dnsServer,gateway,subnet);
  Ethernet.begin(mac);
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  String postData = "{\"value\":" + String(32) + "}";

  client.beginRequest();
  client.post("/");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Authorization", "SharedAccessSignature sr=procheck-prod.azure-devices.net%2Fdevices%2Ftestingnow_avi63&sig=%2Bwgm%2BuRpHnN5jn8px4916zLWMUUxcWjHgmHJcXGH8bQ%3D&se=1685168670");
  client.sendHeader("Content-Length", postData.length());
  client.beginBody();
  client.print(postData);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  client.stop();

  Serial.println(statusCode);             //Prints the HTTP status response - 200 if OK
  Serial.println("Data to the Server");
  Serial.println("Wait one seconds");

  delay(3000);
}
