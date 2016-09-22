// error routine
void menu_routine(int errno)
{
  int i;
  
  for (i=1; i<=3; i++)
  {
    tone(beepPin, 200, 1000);
    delay(1100);
  }
  
  switch (errno){
    case 1: Serial.println("Menu switch set."); break;
    case 2: Serial.println("Accelerometer error."); break;
    case 3: Serial.println("Altimeter error."); break;
    default: break;
  }
  Serial.println();
  while(1) {menu();}
}

void menu()
{  
  int i, value;
  char x;
  String pressureString;
  float pressure;
    
  delay(3000); // 3 seconds to set up terminal program
  Serial.println("Altimeter Menu"); Serial.println();
  Serial.println("   1. Display data");
  Serial.println("   2. Reset flight counter to zero");
  Serial.println("   3. Enter launch date");
  Serial.println("   4. Prepare for launch");
  Serial.println(); Serial.print("Enter option: ");
  
  value = getinput();
  Serial.println(char(value));
  value = int(value) - 48;
 
  switch (value)
  {
    // dump data
    case 1: Serial.println("Display data."); Serial.println(); show_data(); break;
    
    // reset flight counter
    case 2: Serial.println("Reset flight counter.");
            Serial.println("This option will erase any flight data on SD card.");
            Serial.print("Are you sure (y/n)? "); 
            x = char(getinput());
            Serial.println(x);
            if (x == 'y' || x == 'Y')
            {
              deleteFile("FLT*");
              store(0, fltNoAdd);
            }
            break;  
   
  // enter date
    case 3: Serial.print("Month (mm): "); store_date(dateMoAdd);
            Serial.print("Day (dd): "); store_date(dateDaAdd);
            Serial.print("Year (yy): "); store_date(dateYrAdd);
            Serial.print("Date set to: ");Serial.println(showdate());
            break;
               
  // ready for launch
    case 4: Serial.print("Are you sure (y/n)? "); 
            x = char(getinput());
            Serial.println(x);
            if (x == 'y' || x == 'Y')
            {
              Serial.println("Move the menu jumper pin to flight position.");
              while(digitalRead(menuPin) == HIGH){} // stays here until jumper in flight position
              Serial.println("Ready for launch.");
              while(1)
              {
                digitalWrite(LEDPin, HIGH);
                delay(200);
                digitalWrite(LEDPin, LOW);
                delay(200);
               }
             } 
             break;
     // default case
     default: break;
  }
  Serial.println();
}

int getinput()
{
  int incomingByte = 0;
  while(Serial.available() <=0) {}
  {
    // read the incoming byte:
    incomingByte = Serial.read();
    return(incomingByte);
  }
}

void store_date(int addr)
{
  int inputvalue = 0;
  while (inputvalue == 0)
    inputvalue = Serial.parseInt();
  Serial.println(inputvalue);
  store(inputvalue, addr);
}

String showdate()
{
  String dateString = "";
  dateString += String(retrieve(dateMoAdd)); dateString += "-";
  dateString += String(retrieve(dateDaAdd)); dateString += "-";
  dateString += String(retrieve(dateYrAdd));
  return dateString;
}

void show_data()
{
  int flightNo;
  int value = 0;
  char filename[] = "FLT000.txt";
  
  flightNo = retrieve(fltNoAdd);
  if (flightNo == 0)
  {
    Serial.println("There are no flights stored.");
    return;
  }
  Serial.print("There are "); Serial.print(flightNo); Serial.println(" flights stored.");
  Serial.print("Please enter the flight number you wish to display (1-"); Serial.print(flightNo);Serial.print("): ");
  do
  {
    while (value == 0)
      value = Serial.parseInt();
  }
  while ((value < 1) || (value > flightNo));
  Serial.println(value);

  // make a new filename given new flight no.
  int counter, digit;
  counter = value;
  digit = counter / 100;
  filename[3] = char((digit) + int('0'));
  counter %= 100;
  digit = counter / 10;
  filename[4] = char((digit) + int('0'));
  counter %= 10;
  digit = counter;
  filename[5] = char((digit) + int('0'));
  
  display_data(filename);
}


