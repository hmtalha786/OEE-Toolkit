// Analog Input
int Input_1; 

// Flow Value in Kg/h
float F1;    

// Change resistor value as per the site
int R_Val = 500;

// Max Flow of meter
float Max_Flow = 1300;

// Flow in gram/sec
float F_GPS = 0;

// Totalizer of flow
int Total_Flow = 0;

// Incremental index
int n = 0;

//---------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600UL);
}

//---------------------------------------------------------------------------------------

void loop() {

  // Read Analog Input
  Input_1 = analogRead(I0_12);

  // Scale Analog value to flow scale
  F1 = map( Input_1, 0, 1023, 0, Max_Flow );

  // Convert into g/s
  F_GPS = F1 / 3.6;

  // Totalizer
  Total_Flow = F_GPS + Total_Flow;

  // Delay of 1 Second
  delay(1000);

  // Incremet Index
  n++;

  // Reset clock after 60 seconds
  if ( n >= 60 ) {

    Serial.print("Total Flow : ");
    Serial.println(Total_Flow);

    // Reset Values
    Total_Flow = 0;
    n = 0;

  }

}
