#include <ArduinoJson.h>

int val1 = 0;
int val2 = 0;
int val3 = 0;
int val4 = 0;
int val5 = 0;
int val6 = 0;
double CDowntime = 0;
unsigned long stdowntime = 0;
unsigned long temp = 0;
unsigned long tempMid = 0;
unsigned long level = 0;
double tempmin = 0;
double tempminMid = 0;
double level1 = 0;
unsigned long i = 0;
unsigned long ii = 0;
unsigned long k = 0;
unsigned long j = 0;
unsigned long jj = 0;

unsigned long checkkk = 0;

int iii = 0;
int jjj = 0;
int sss = 0;
int ss = 0;
int p = 0;
int q = 0;
int r = 0;
int v = 0;

int prev1 = 0;
int prev2 = 0;
int prev3 = 0;
int prev4 = 0;
int prev5 = 0;
int prev6 = 0;

unsigned long count1 = 0;
unsigned long count2 = 0;
unsigned long count3 = 0;
unsigned long count4 = 0;
unsigned long count5 = 0;
unsigned long count6 = 0;

unsigned long s = 0;
unsigned long s1 = 0;
unsigned long s2 = 0;
unsigned long z = 0;
unsigned long z1 = 0;
unsigned long z2 = 0;

int dt = 0;
int dt1 = 0;
int dt2 = 0;

volatile byte state = LOW;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(500000);
  //  Serial3.begin(115200);
  //digitalWrite(51, LOW);
  pinMode(I0_12, INPUT);
  pinMode(I0_11, INPUT);
  pinMode(I0_10, INPUT);
  pinMode(I0_9, INPUT);
  pinMode(I0_8, INPUT);
  pinMode(I0_7, INPUT);
  pinMode(Q0_4, OUTPUT);
  ss = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
    checkkk = millis();
  val1 = digitalRead(I0_12);
  //Sensor 1
  if ((prev1 - val1) == 1) //((val - x) > 100)
  {
    count1++;
    delay(3);
    z = millis();
    p = 2;
  }
  prev1 = val1;
  //..............................................................................
  val2 = digitalRead(I0_11);
  //Sensor 2
  if ((prev2 - val2) == 1) //((vall - xx) > 100)
  {
    count2++;
    delay(3);
  }
  prev2 = val2;
  //..............................................................................
  val3 = digitalRead(I0_10);
  //Sensor 3
  if ((prev3 - val3) == 1) //((vall - xx) > 100)
  {
    count3++;
    delay(3);
    dt1 = 0;
    z1 = millis();
  }
  prev3 = val3;
  //..............................................................................
  val4 = digitalRead(I0_9);
  //Sensor 4
  if ((prev4 - val4) == 1) //((vall - xx) > 100)
  {
    count4++;
    delay(3);
  }
  prev4 = val4;
  //..............................................................................
  val5 = digitalRead(I0_8);
  //Sensor 5
  if ((prev5 - val5) == 1) //((vall - xx) > 100)
  {
    count5++;
    delay(3);
    dt2 = 0;
    z2 = millis();
  }
  prev5 = val5;
  //..............................................................................
  val6 = digitalRead(I0_7);
  //Sensor 6
  if ((prev6 - val6) == 1) //((vall - xx) > 100)
  {
    count6++;
    delay(3);
  }
  prev6 = val6;
  //..............................................................................
  //Sensor Status 3
  s1 = (millis() - z1);
  if (s1 > 32750)
  {
    z1 = millis();
    dt1 = 1;
  }
  //................................................................................
    //Sensor Status 5
  s2 = (millis() - z2);
  if (s2 > 32750)
  {
    z2 = millis();
    dt2 = 1;
  }
  //................................................................................
  s = (millis() - z);

  if (s > 32750)
  {
    //Serial.println("Downtime Starts");
    z = z + 100000000;
    v = dt;
    dt = 1;
    if (-1 == v - dt)
    { stdowntime = millis();
      stdowntime = stdowntime - 32750;
    }
    q = 2;
    p = 0;
  }

  if ((p == 0) && (q == 2))
  {
    //  Serial.println("While in Downtime");
    tempMid = millis();
    tempMid = tempMid - stdowntime;
    //Serial.println(tempMid);
    tempminMid = (double)tempMid / 60000.0;
    CDowntime = CDowntime + tempminMid - level1;
    level = tempMid;
    level1 = (double)level / 60000.0;
  }

  if ((p == 2) && (q == 2))
  {
    //  Serial.println("Downtime Ends");
    p = 0;
    q = 0;
    r = dt;
    dt = 0;
    if (1 == r - dt)
    { //temp = millis();
      //    if(temp < stdowntime){temp = 0;stdowntime = 0;}
      //    temp = temp - stdowntime;
      //    tempmin = (double)temp / 60000.0;
      //    CDowntime = CDowntime + tempmin;
      level = 0;
      level1 = 0;
    }
  }

  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject();

  // JsonArray& timestamps = JSONencoder.createNestedArray("times"); //JSON array .......................
  //    timestamps.add("10:10"); //Add value to array
  JSONencoder["SR1"] = count1;
  JSONencoder["SR2"] = count2;
  JSONencoder["SR3"] = count3;
  JSONencoder["SR4"] = count4;
  JSONencoder["SR5"] = count5;
  JSONencoder["SR6"] = count6;
  JSONencoder["SS1"] = dt;
  JSONencoder["SS3"] = dt1;
  JSONencoder["SS5"] = dt2;
//  JSONencoder["Cumulative Downtime"] = CDowntime;

//  JSONencoder["SR1"] = 123456789012;
//  JSONencoder["SR2"] = 123456789012;
//  JSONencoder["SR3"] = 123456789012;
//  JSONencoder["SR4"] = 123456789012;
//  JSONencoder["SR5"] = 123456789012;
//  JSONencoder["SR6"] = 123456789012;
//  JSONencoder["S1S"] = 0;
//  JSONencoder["S3S"] = 0;
//  JSONencoder["S5S"] = 0;
  //JSONencoder["CD"] = CDowntime;

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);

  sss = (millis() - ss);
  //Serial.println(sss);
  if (sss > 10000 && sss < 10035)
  {
    Serial.print(JSONmessageBuffer);
    Serial.print('A');

    digitalWrite(Q0_4, HIGH);
    delay (2);
    digitalWrite(Q0_4, LOW);
    ss = millis();
  }
}
