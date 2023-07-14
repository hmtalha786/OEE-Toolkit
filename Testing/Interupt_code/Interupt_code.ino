volatile int rpmcount = 0;
unsigned long rpm = 0;
unsigned long lastmillis = 0;
//char data = '0';

void setup() {
  Serial.begin(115200);
  attachInterrupt(0, rpm_fan, HIGH);                                //interrupt cero (0) is on pin two(2).
}

void loop() {
  //  data = Serial.read();
  //  pwmdata(data);
  if (millis() - lastmillis == 60) {                                    /*Uptade every one second, this will be equal to reading frecuency (Hz).*/
    detachInterrupt(0);                                             //Disable interrupt when calculating
    rpm = rpmcount * (1000 / 60);                                       /* Convert frecuency to RPM, note: this works for one interruption per full rotation.
                                                                           For two interrups per full rotation use rpmcount * 30.*/
    rpmcount = 0;                                                   // Restart the RPM counter
    lastmillis = millis();                                          // Uptade lasmillis
    //char    inByte = Serial.read();                               // get incoming byte:
    //if (inByte=='D')                                              //check if data is recieved correctly
    {
      Serial.print("DSU");
      Serial.print(rpm);
      Serial.println("DSU");
    }
    attachInterrupt(0, rpm_fan, HIGH);                              //enable interrupt
  }
}

void rpm_fan() {
  /* this code will be executed every time the interrupt 0 (pin2) gets low.*/
  rpmcount++;
}
