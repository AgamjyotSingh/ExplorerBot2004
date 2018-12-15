#include<SoftwareSerial.h>
#define rxPin 10
#define txPin 11
#include "roombaDefines.h"
#include <Sweep.h>

SoftwareSerial softSerial = SoftwareSerial(rxPin,txPin);

SoftwareSerial BT1(12, 13);

Sweep device(Serial1);

const uint16_t DIST_CM_MIN = 1;   // (cm) Clip range values on the low end
const uint16_t DIST_CM_MAX = 100; // (cm) Clip range values on the high end
const uint16_t DIST_CM_RANGE = DIST_CM_MAX - DIST_CM_MIN;
const uint16_t BLINK_INTERVAL_MS_MIN = 100;  // (milliseconds) Min blink interval, indicates max distance
const uint16_t BLINK_INTERVAL_MS_MAX = 2000; // (milliseconds) Max blink interval, indicates min distance
const uint16_t BLINK_INTERVAL_MS_RANGE = BLINK_INTERVAL_MS_MAX - BLINK_INTERVAL_MS_MIN;

// the angular range (Field of View). The distance of the shortest valid ranging in the
// angular window between [360-FOV: 0] degrees will be used.
// (0 deg corresponds to the thin groove + LED on the face of the sweep device)
const uint8_t FOV = 30;

// Finite States for the program sequence
const uint8_t STATE_WAIT_ON_RESET = 0;
const uint8_t STATE_ADJUST_DEVICE_SETTINGS = 1;
const uint8_t STATE_VERIFY_CURRENT_DEVICE_SETTINGS = 2;
const uint8_t STATE_BEGIN_DATA_ACQUISITION = 3;
const uint8_t STATE_GATHER_DATA = 4;
const uint8_t STATE_UPDATE_BLINK_FREQUENCY = 5;
const uint8_t STATE_RESET = 6;
const uint8_t STATE_ERROR = 7;
const uint8_t STATE_MANUAL_CONTROL = 8;
const uint8_t STATE_autonomous = 9;


uint16_t closestDistanceInSpecifiedFOV = 4000; // the distance (in cm) of the closest object in the specified angular range
uint16_t interval = 1000;                      // interval at which to blink onboard LED (milliseconds)
int ledState = LOW;                            // ledState used to set the LED
unsigned long previousMillis = 0;              // time of the last LED update

// Current state in the program sequence
uint8_t currentState = STATE_WAIT_ON_RESET;


void setup() {

 
 delay(1000);

 pinMode(rxPin, INPUT);
 pinMode(txPin, OUTPUT);

 BT1.begin(9600);
 BT1.println("Hello from Arduino");

 pinMode(LED_BUILTIN, OUTPUT);
 digitalWrite(LED_BUILTIN, LOW);

 softSerial.begin(57600);

 Serial.begin(57600);

 softSerial.write(128);
 softSerial.write(131);
 
 Serial1.begin(115200);

 reset1();

}

char c;

void loop() {
  BT1.listen();
  if(BT1.available()){
   c = BT1.read();
   Serial.print(c);
   if(c == 'm'){
    movement(c);
   }
  }
  if(c == 'p'){
    switch (currentState)
  
  {

    
  case STATE_WAIT_ON_RESET:
    currentState = waitOnReset() ? STATE_ADJUST_DEVICE_SETTINGS : STATE_ERROR;

    break;
    
  case STATE_ADJUST_DEVICE_SETTINGS:
    currentState = adjustDeviceSettings() ? STATE_VERIFY_CURRENT_DEVICE_SETTINGS : STATE_ERROR;
    break;
    
  case STATE_VERIFY_CURRENT_DEVICE_SETTINGS:
    currentState = verifyCurrentDeviceSettings() ? STATE_BEGIN_DATA_ACQUISITION : STATE_ERROR;
    break;
    
  case STATE_BEGIN_DATA_ACQUISITION:
    // Attempt to start scanning (will wait for motor speed to stabilize and calibration routine to complete...)
    currentState = device.startScanning() ? STATE_GATHER_DATA : STATE_ERROR;
    break;

  case STATE_GATHER_DATA:
    if (gatherDistanceInfo())
      currentState = STATE_UPDATE_BLINK_FREQUENCY; 
    break;
    
  case STATE_UPDATE_BLINK_FREQUENCY:
    updateBlinkFrequency();
    // reset the closest distance
    closestDistanceInSpecifiedFOV = 4000;
    currentState = STATE_GATHER_DATA;
    break;
    
  case STATE_RESET:
    reset1();
    currentState = STATE_WAIT_ON_RESET;
    break;

  default: // there was some error
    // DO NOTHING
    interval = 10; // short enough interval to make the LED appear as if it is ON
    break;
  }
  
  updateLED();
}
}

 // Waits ~8 seconds for the device to reset and verifies it can communicate
bool waitOnReset()
{
  int tempState = LOW;
  for (int i = 0; i < 16; i++)
  {
    delay(500);
    // toggle the LED
    tempState = (tempState == LOW) ? HIGH : LOW;
    digitalWrite(LED_BUILTIN, tempState);
  }

  // to verify communication, check a random command response that doesn't require motor is ready
  return device.getSampleRate() > 0;
}

// Adjusts the device settings
bool adjustDeviceSettings()
{
  // Set the motor speed to 2HZ (codes available from 1->10 HZ)
  bool bSuccess = device.setMotorSpeed(MOTOR_SPEED_CODE_3_HZ);

  /* 
   *  Device will always default to 500HZ scan rate when it is powered on.
   *  Snippet below is left for reference.
   */
  // Set the sample rate to 500HZ (codes available for 500, 750 and 1000 HZ)
  //bool bSuccess = device.setSampleRate(SAMPLE_RATE_CODE_500_HZ);
  return bSuccess;
}

// Querries the current device settings (motor speed and sample rate)
// and prints them to the console
bool verifyCurrentDeviceSettings()
{
  // Read the current motor speed and sample rate
  int32_t currentMotorSpeed = device.getMotorSpeed();
  if (currentMotorSpeed < 0)
    return false;
  int32_t currentSampleRate = device.getSampleRate();
  if (currentSampleRate < 0)
    return false;
  return true;
}

// Gathers distance info, for the angular region [360-FOV: 0] degree
// returns true if distances for the complete range [360-FOV: 0] have been collected
bool gatherDistanceInfo()
{
  // attempt to get the next scan packet
  // Note: getReading() will return a ScanPacket object
  bool success = false;
  ScanPacket reading = device.getReading(success);
  if (success)
  {
    // marks the end of the angular range, so report true
    if (reading.isSync())
      return true;

    // consider a Field of View in the angular range [360-FOV: 0]
    if (reading.getAngleDegrees() > 330 - FOV)
    {
      // only consider valid readings (sensor will report distance of 1 for failed readings)
      uint16_t dist = reading.getDistanceCentimeters();
      if (dist > 1)
      {
        // check if this reading is closer than anything seen so far
        if (dist < closestDistanceInSpecifiedFOV)
          closestDistanceInSpecifiedFOV = dist;
      }
      
    }
    
  }
  
  return false;
}
  





// Updates the blink frequency based off the average distance around the azimuth 0 degree mark
void updateBlinkFrequency()
{
  // clip the distance
  uint16_t dist = max(closestDistanceInSpecifiedFOV, DIST_CM_MIN);
  dist = min(dist, DIST_CM_MAX);

  // normalize the average
  float normalized = (1.0 * DIST_CM_MAX - dist) / DIST_CM_RANGE;

  // calculate a blink interval (in ms) from the normalized distance
  interval = (uint16_t)(BLINK_INTERVAL_MS_MIN + normalized * BLINK_INTERVAL_MS_RANGE);
  if(interval < 1000){
    driveWheels(100, 100);
  }
  else{
    driveWheels(50, -50);
    delay(2000);
  }


}

// update the LED if necessary (if the difference between the current
// time and last update time is greater thant the desired interval)
void updateLED()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // store the timestamp
    previousMillis = currentMillis;

    // determine the new State
    ledState = (ledState == LOW) ? HIGH : LOW;

    // toggle the LED
    digitalWrite(LED_BUILTIN, ledState);
  }
}

// Resets the variables and state so the sequence can be repeated
void reset1()
{
  currentState = STATE_WAIT_ON_RESET;
  closestDistanceInSpecifiedFOV = 4000;
  ledState = LOW;
  digitalWrite(LED_BUILTIN, ledState); // turn the LED off by making the voltage LOW
  device.reset();
}
