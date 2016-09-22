// beeps j times
void beeptimes(int j)
{
  int i;
  for (i=1; i<=j; i++)
  {
    tone(beepPin, 3000, 200);
    delay(500);
  }
}
    
// Sends out beep-coded-decimal for c
void countout(int c)
{
  int x;
  boolean First = true;
  int digit=10000;
  while (digit > 0)
  {
    x = c / digit;
  
    if ((!First) && (x ==0)) { beeptimes(10); delay(1000);}
    else {  if (x > 0) {beeptimes(x); delay(1000);} }
    if ((First) && (x > 0)){ First = false; }
    c = c % digit;
    digit = digit / 10;
  }
}

// store and retrieve routines
void store(int z, int bytecounter)
// stores integer in two bytes of EEPROM starting at bytecounter
{
  EEPROM.write(bytecounter, highByte(z));
  bytecounter++;
  EEPROM.write(bytecounter, lowByte(z));
}

int retrieve(int bytecounter)
// retrieves integer from two bytes of EEPROM starting at bytecounter
{
  byte high = EEPROM.read(bytecounter);
  bytecounter++;
  byte low = EEPROM.read(bytecounter);
  int z = (high << 8) + low;
  return z;
}


