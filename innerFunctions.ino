void playSound (int num) 
{
  switch (num)
  { 
    case 1: 
      softSerial.write("\x8c\x01\x04\x42\x20\x3e\x20\x42\x20\x3e\x20"); // [140] [1] [4] [68] [32] ... place "start sound" in slot 1
      softSerial.write("\x8d\x01"); // [141] [1] play it (in slot 1)
      break;
 
    case 2: 
      softSerial.write("\x8c\x01\x01\x3c\x20"); // place "low freq sound" in slot 2
      softSerial.write("\x8d\x01"); // play it (in slot 2)
      break;

    case 3:
      softSerial.write("\x8c\x01\x20\x48\x20"); // place "high freq sound" in slot 3
      softSerial.write("\x8d\x02"); // play it (in slot 3)
      break;

    case 4:
      softSerial.write("\x8c\x01\x0e\x3e\x10\x43\x0c\x46\x04\x45\x10\x43\x20\x4a\x10\x48\x18\x45\x18\x43\x0c\x46\x10\x45\x20\x41\x10\x44\x20");
      softSerial.write("\x8d\x01");
      break;    

    case 5:
      softSerial.write("\x8c\x01\x09\x58\x10\x57\x10\x58\x10\x57\x10\x58\x10\x53\x10\x56\x10\x54\x10\x51\x10");                         
      softSerial.write("\x8d\x01");    
      break;

  }
}


 void seekDock(void)
{
  softSerial.write(143);
}

void powerOff(void)
{
  softSerial.write(133);
}

void stop(void)
{
  softSerial.write(173);
}

void startFull()
{  
  softSerial.write(128); 
  softSerial.write(132);  
  delay(1000);
}

void startSafe()
{  
  softSerial.write(128); 
  softSerial.write(131);
  delay(1000);
}

void reset(void)
{
  softSerial.write(7);
}
