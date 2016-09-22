/*
 Connect the following Arduino to OpenLog:
 2 to RXI
 3 to TX0
 4 to GRN
 3.3V to VCC
 GND to GND
 
 For both the i2C devices, accelerometer and barometer
 GND to GND
 3.3V to VCC
 A4 to SDA
 A5 to SCL
 
 For buzzer:
 6 to Cathode
 GND to Anode
 
 For LED:
 7 to resistor on Cathode
 GND to Anode
 
 For menu swx:
 8 to Center
 GND to Side 1
 3.3V or 5V to Side 2 
*/

// include generic libraries
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h> // for ADXL 345 accelerometer
#include <SparkFunT5403.h> // for altimeter
#include <EEPROM.h> // to read and write to EEPROM

// include local modules
#include "Rocket.h"
#include "Menu.h"
#include "OpenLog.h"
#include "Storage_Modules.h"

// define fixed values
#define menuPin 8
#define resetOpenLog 4
#define beepPin 6
#define LEDPin 7
#define liftoff_ht 20 // test for liftoff by 20 feet
#define maxBytes 0 // start of EEPROM address space

// define global constants - calculated just once, these are EEPROM addresses
const int fltNoAdd = maxBytes;
const int dateMoAdd = maxBytes + 2;
const int dateDaAdd = maxBytes + 4;
const int dateYrAdd = maxBytes + 6;

// define global variables
float groundLevelPressure;
int apogee;

/* Assign a unique ID to the accelerometer, so no conflict with barometer */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345); // for accelerometer

T5403 barometer(MODE_I2C);  //attaching the barometer in i2C mode

/* Set up OpenLog as software defined serial device */
SoftwareSerial OpenLog(3, 2); //Soft RX on 3, Soft TX out on 2

/* The following required for the Adafruit sensor library used for the ADXL345 */
sensor_t sensor;
sensors_event_t event;

void setup(void) 
{
  int systemerrors, flightNo, feet, Xgees, Ygees, Zgees, landedCount;
  int a1, a2, a3; // used to catch outliers for apogee
  boolean launched, landed;

  long starttime;

  float elapsedtime, fahrenheit;
  char filename[] = "FLT000.txt";
    
  initialize(); // initialize i/o and pins
   
  systemerrors = systemtest(); // tests to see if menu switch is on, tests and initializes sensors
  if (systemerrors != 0) // if there is a system error, go to menu routine
    menu_routine(systemerrors);
    
// no system errors, all systems go!
  
  digitalWrite(LEDPin, HIGH);  // LED on solid until ready to launch
  
// Get relevant launch data
  fahrenheit = gettemperature();
  groundLevelPressure = getgroundLevelPressure();

// initialize variables
  landed = false;
  launched = false;
  landedCount = 0;
  a1 = 0; a2 = 0; a3 = 0;
  
// Increment flight no.
  flightNo = retrieve(fltNoAdd);
  flightNo++;
  
// make a new filename given new flight no.
  int counter, digit;
  counter = flightNo;
  digit = counter / 100;
  filename[3] = char((digit) + int('0'));
  counter %= 100;
  digit = counter / 10;
  filename[4] = char((digit) + int('0'));
  counter %= 10;
  digit = counter;
  filename[5] = char((digit) + int('0'));
  createFile(filename);
  
// store the new flight number
  store(flightNo, fltNoAdd);
  
// File created, OpenLog operative, give three beeps
  noTone(beepPin); delay(2000); countout(3);

// here you put in delay until ready to launch
  
// print data headers
  OpenLog.print("Time\t"); OpenLog.print("Alt.\t"); OpenLog.print("Xgees\t"); OpenLog.print("Ygees\t"); OpenLog.println("Zgees");
  
// Now ready to launch, turn off LED until launch detected
  digitalWrite(LEDPin, LOW);
// here you put in siren() to indicate we are in launch detect mode
  
  starttime = millis(); // initialize start time, will reset on launch detect
  
  
// BEGIN MAIN LOOP
  do
  {
    feet = getfeet();
        
    if (!launched) // if not launched, enter launch detect routine
    {
      if (feet > liftoff_ht) // launch detected
      {
        launched = true;
        digitalWrite(LEDPin, HIGH);  // Turn LED on solid to indicate liftoff is detected
        starttime = millis();
      }
    }
    
    if (launched) // after launch detected, write data, catch apogee values and test for landing
    {
      Xgees = getXgees();
      Ygees = getYgees();
      Zgees = getZgees();
      elapsedtime = (millis() - starttime) / 1000.0;
      write_data(elapsedtime, feet, Xgees, Ygees, Zgees);
      
      // catching apogee values in a1, a2, a3
      if (feet > a1)
        a1 = feet;
      if ((feet < a1) && (feet > a2))
        a2 = feet;
      if ((feet < a1) && (feet < a2) && (feet > a3))
        a3 = feet;

      // test for landing
      if (feet < 2) {landedCount++;}
      if (feet >= 2) {landedCount = 0;}
      if  (landedCount == 3) // landing confirmed with three subsequent readings less than two feet
        landed = true;
    }

    // delay 5 milliseconds before getting next reading  
    delay(5);
  }     
  while(!landed); // loop until landed
// END MAIN LOOP

  apogee = a3; // throw out a1 and a2 as outliers
    
// write out summary
  OpenLog.println();
  OpenLog.println("Summary:");
  OpenLog.print("Date: "); OpenLog.println(showdate());
  OpenLog.print("Launch temp.: "); OpenLog.print(fahrenheit); OpenLog.println(" deg. F.");
  OpenLog.print("Flight duration: "); OpenLog.print(elapsedtime); OpenLog.println(" sec.");
  OpenLog.print("Apogee: "); OpenLog.print(apogee); OpenLog.println(" ft.");

  closeOpenLog();

// clean up miscellaneous log files
  setupOpenLog();
  cleanup();
  closeOpenLog();
}
   
void loop(void) 
{
  siren();
  delay(3000);
  countout(apogee);
  delay(3000);
}
