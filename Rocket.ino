// Initialize serial i/o and pins
void initialize()
{
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT);
  pinMode(beepPin, OUTPUT);
  pinMode(resetOpenLog, OUTPUT);
  pinMode(menuPin, INPUT);
  digitalWrite(LEDPin, LOW); // LEDPin off until launch detect
}

// System test
int systemtest()
{
  boolean good;
  int i;
  int error = 0;
  
// If menuPin is LOW, go to menu
  if (digitalRead(menuPin) == LOW)
    error = 1;

// menu not set
/* Initialise the accelerometer */
  accel.getSensor(&sensor); // attaching the accelerometer
  
// if there was a problem detecting the ADXL345, try again three times  
  if(!accel.begin())
  {
    for (i=1; i< 3; i++)
    {
      good = accel.begin();
      delay(50);
    }
  }
  if (!good) 
    error = 2;

  // accelerometer okay, so set range of 16G for accelerometer
  accel.setRange(ADXL345_RANGE_16_G); // for ADXL 345
  delay(50);

/* Initialise the altimeter */
  barometer.begin();
  
  return error;
}

// general siren routine
void siren()
{
  delay(1000);
  tone(beepPin, 3000, 3000);
  delay(1000);
}

// returns ground level pressure in pascals as float
float getgroundLevelPressure()
{
  float glp = 0.0;
  float accglp = 0.0;
  int i;
  
  barometer.begin();
  for(i=0; i< 10; i++) // repeat 10 times for accuracy
  {
    do
    {
      glp = barometer.getPressure(MODE_ULTRA);
      delay(100);
    }
    while(glp <= 0.0);
    accglp = accglp + (glp/100.0); // need to accumulate in pascals/100 because number is large
  }
  glp = accglp * 10.0; // to get (10 accumulations of pascals/100) -> pascals
  return glp;
}

// returns launch temperature in degrees fahrenheit as float
float gettemperature()
{
  float f;
  barometer.begin();
  f = barometer.getTemperature(FAHRENHEIT) / 100.0;
  return f;
}
  
// returns altitude AGL in meters as float
float getaltitude()
{
  float P, P0, alt;
  float y;
  
  barometer.begin();
  P = barometer.getPressure(MODE_HIGH) /100.0;
  P0 = groundLevelPressure / 100.0;
  alt = 44330.0*(1-pow(P/P0,1/5.255));
  
  return alt;
}
  
//returns altitude AGL in feet as integer
int getfeet()
{
  int xfeet;
  float height;
  height = getaltitude();
  xfeet = (height * 3.2808) + 0.5;
 
  return xfeet;
}
    
//returns acceleration in x dimension that is 100 X actual gees as integer
int getXgees()
{
  float xaxis;
  int xgees;
  accel.getSensor(&sensor); // attaching the accelerometer
  accel.getEvent(&event);

  xaxis = event.acceleration.x; // meters per second squared
  xgees = 100.0 * (xaxis / 9.81) + 0.5; // 9.81 meters per second squared is one gee - returning 100 * gees
  
  return xgees;
}

//returns acceleration in y dimension that is 100 X actual gees as integer
int getYgees()
{
  float yaxis;
  int ygees;
  accel.getSensor(&sensor); // attaching the accelerometer
  accel.getEvent(&event);

  yaxis = event.acceleration.y; // meters per second squared
  ygees = 100.0 * (yaxis / 9.81) + 0.5; // 9.81 meters per second squared is one gee - returning 100 * gees
  
  return ygees;
}

//returns acceleration in z dimension that is 100 X actual gees as integer
int getZgees()
{
  float zaxis;
  int zgees;
  accel.getSensor(&sensor); // attaching the accelerometer
  accel.getEvent(&event);

  zaxis = event.acceleration.z; // meters per second squared
  zgees = 100.0 * (zaxis / 9.81) + 0.5; // 9.81 meters per second squared is one gee - returning 100 * gees
  
  return zgees;
}

/*
// returns instantaneous velocity in feet per second as float
float getvelocity(float tt, int ss)
{
  int i;
  float delta_s, delta_t, inst_vel;
  
// push down top 5 buffered elements as a stack
  for (i=1; i<= 5; i++)
  {
    t[i-1] = t[i];
    s[i-1] = s[i];
  }
  
// add new feet and time to the top of the stack
  t[5] = tt;
  s[5] = ss;
  
// calculate delta s and delta t for new altitude and time, going back five points
  delta_t = t[5] - t[1]; // t[i] is a float
  delta_s = float(s[5] - s[1]); // because s[i] is an integer
  
// instantaneous velocity in feet per second
  inst_vel = (delta_s/delta_t);
  
// return feet per second
  return (inst_vel);
}
*/
