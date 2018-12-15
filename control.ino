void drive(int velocity, int radius)
{
 clamp(velocity,-500,500);
 clamp(radius, -2000,2000);
  
  softSerial.write(137);
  softSerial.write(velocity >> 8);
  softSerial.write(velocity);
  softSerial.write(radius >> 8);
  softSerial.write(radius);
  
}

void driveWheels(int right, int left)
{
  clamp(right, -500, 500);
  clamp(left, -500, 500);

  softSerial.write(145);
  softSerial.write(right >> 8);
  softSerial.write(right);
  softSerial.write(left >> 8);
  softSerial.write(left);
}

void driveWheelsPWM(int rightPWM, int leftPWM)
{
  clamp(rightPWM, -255, 255);
  clamp(leftPWM, -255, 255);

  softSerial.write(146);
  softSerial.write(rightPWM >> 8);
  softSerial.write(rightPWM);
  softSerial.write(leftPWM >> 8);
  softSerial.write(leftPWM);
}

void turnCW(unsigned short velocity, unsigned short degrees)
{
  drive(velocity, -1);
  clamp(velocity, 0, 500);
  delay(6600);
  drive(0,0);
 
}

void turnCCW(unsigned short velocity, unsigned short degrees)
{
  drive(velocity, 1);
  clamp(velocity, 0, 500);
  delay(6600);
  drive(0,0);
  
}

void driveStop(void)
{
  drive(0,0);
}

void motorSquareTest(void)
{
 driveWheels(200,200);
 delay(5000);
 driveStop();
 turnCCW(30, 90);

 driveWheels(200,200);
 delay(5000);
 driveStop();
 turnCCW(30, 90);

 driveWheels(200,200);
 delay(5000);
 driveStop();
 turnCCW(30, 90);

 driveWheels(200,200);
 delay(5000);
 driveStop();
 turnCCW(30, 90);
 
}

void moveWave(void)
{
  driveWheels(200,40);
  delay(2500);
  driveWheels(40,200);
  delay(2500);
  
}

void driveRight(int right)
{
  driveWheels(0, right);
}

void driveLeft(int left)
{
  driveWheels(left, 0);
}
