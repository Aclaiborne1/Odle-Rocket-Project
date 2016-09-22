// write data routine - may need delay adjustment
void write_data(float time, int feet, int Xgees, int Ygees, int Zgees)
{
  OpenLog.print(time); OpenLog.print('\t');
  OpenLog.print(feet); OpenLog.print('\t');
  OpenLog.print(Xgees); OpenLog.print('\t');
  OpenLog.print(Ygees); OpenLog.print('\t');
  OpenLog.println(Zgees);
}

// sets OpenLog for writing
void setupOpenLog(void) 
{
  pinMode(resetOpenLog, OUTPUT);
  OpenLog.begin(9600);

  //Reset OpenLog
  digitalWrite(resetOpenLog, LOW);
  delay(100);
  digitalWrite(resetOpenLog, HIGH);

  //Wait for OpenLog to respond with '<' to indicate it is alive and recording to a file
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '<') break;
  }
}

// creates a file named "filename" and initializes it for appending 
void createFile(char *fileName) 
{
  setupOpenLog();
  gotoCommandMode();
  OpenLog.print("new ");
  OpenLog.print(fileName);
  OpenLog.write(13); //This is \r
 
   //Wait for OpenLog to return to waiting for a command
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '>') break;
  }

  OpenLog.print("append ");
  OpenLog.print(fileName);
  OpenLog.write(13); //This is \r
 
  //Wait for OpenLog to indicate file is open and ready for writing
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '<') break;
  }
  //OpenLog is now waiting for characters and will record them to the new file  
}

// deletes a file named "fileName"
void deleteFile(char *fileName) 
{
  setupOpenLog();
  gotoCommandMode();
  OpenLog.print("rm ");
  OpenLog.print(fileName);
  OpenLog.write(13); //This is \r

   //Wait for OpenLog to return to waiting for a command
  while(1) {
    if(OpenLog.available())
      if(OpenLog.read() == '>') break;
  }
}

// directs OpenLog to enter command mode
void gotoCommandMode(void) {
  //Send three control z to enter OpenLog command mode
  //Works with Arduino v1.0
  do
  {
    OpenLog.write(26);
    OpenLog.write(26);
    OpenLog.write(26);
    delay(100);
  }
  while(!OpenLog.available());
  while(OpenLog.read()!='>'){}
}

// gets rid of miscellaneous logfiles
void cleanup()
{
  deleteFile("LOG*");
}

// closes out OpenLog - flushes buffer
void closeOpenLog()
{
  delay(1000);
  digitalWrite(resetOpenLog, LOW);
  delay(1000);
  digitalWrite(resetOpenLog, HIGH);
}

// displays all the contents of a file named "fileName"
void display_data(char *fileName)
{
  int i;

  setupOpenLog();
  gotoCommandMode();

  OpenLog.print("read ");
  OpenLog.print(fileName);
  OpenLog.print(" 0 1000000 3");
  OpenLog.write(13);
  
  while(1) 
  {
    if(OpenLog.available())
      if(OpenLog.read() == '\r') break;
  }
   
  for(int timeOut = 0 ; timeOut < 1000 ; timeOut++) 
  {
    while(OpenLog.available())
      Serial.write(OpenLog.read());
  }
  Serial.println();
}

