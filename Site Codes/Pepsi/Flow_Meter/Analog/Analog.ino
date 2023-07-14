//int inMin = 0;
//int inMax = 1023;
//int outMin = 0;
//int outMax = 1300;

//  float flow = ( input ) / ( inMax * outMax );

void setup() {
  Serial.begin(9600UL);
}

void loop() {

  int input = analogRead(I0_12);

  float volt = map(input, 0, 1023, 0, 10000);
  float curr = map(input, 0, 1023, 0, 20000);
  float flow = map(input, 0, 1023, 0, 1300);

  String a = "\"Analog Resolution\":" + String(input) + " bits" + "," ;
  
  String v = "\"Voltages\":" + String(volt/1000) + " volts" + "," ;
  
  String c = "\"Current\":" + String(curr/1000) + " mA" + "," ;
  
  String f = "\"Flow Value\":" + String(flow) + " kg/h" ;

  String json = "{" + a + v + c + f + "}" ;
  Serial.println(json);
  Serial.println("");

  delay(1000);
}
