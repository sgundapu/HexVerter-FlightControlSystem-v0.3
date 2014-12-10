// We'll use Servo to control our ESCs
#include <Servo.h> 
// Libraries for interaction with IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_10DOF.h>

// PWM pins
#define ESCPin1_TT 3
#define ESCPin2_LT 5
#define ESCPin3_TB 6
#define ESCPin4_RB 9
#define ESCPin5_RT 10
#define ESCPin6_LB 11

// Here we declare the ESCs,... we treat them as servos
Servo ESC_RT;
Servo ESC_LT;
Servo ESC_TT;
Servo ESC_RB;
Servo ESC_LB;
Servo ESC_TB;

int PWMMax = 160;  // maximum PWM value
int PWMMin = 10;   // minimum PWM value that causes the motors to spin
int PWMZero = 0;   // PWM zeroing value used to calibrate the ESC

// Assign a unique ID to the sensors
Adafruit_10DOF                dof   = Adafruit_10DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);

/* Update this with the correct SLP for accurate altitude measurements */
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

// Orientation that we get from sensors
sensors_vec_t   orientation;

// Init all the sensors of our IMU
void initSensors()
{
  if(!accel.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP180 ... check your connections */
    Serial.println("Ooops, no BMP180 detected ... Check your wiring!");
    while(1);
  }
}

void getSensorData(){
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_event_t bmp_event;
  
  // Calculate pitch and roll from the raw accelerometer data
  accel.getEvent(&accel_event);
  if (dof.accelGetOrientation(&accel_event, &orientation))
  {
    /* 'orientation' should have valid .roll and .pitch fields */
    Serial.print(F("Roll: "));
    Serial.print(orientation.roll);
    Serial.print(F("; "));
    Serial.print(F("Pitch: "));
    Serial.print(orientation.pitch);
    Serial.print(F("; "));
  }
  
  /* Calculate the heading using the magnetometer */
  mag.getEvent(&mag_event);
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
  {
    /* 'orientation' should have valid .heading data now */
    Serial.print(F("Heading: "));
    Serial.print(orientation.heading);
    Serial.print(F("; "));
  }
}

void setup(){
  delay(10000);
  
  // Setup the connection to the XBee
  Serial.begin(9600);

  // Connect to the ESCs as if they were servos
  ESC_RT.attach(ESCPin5_RT);
  ESC_LT.attach(ESCPin2_LT);
  ESC_TT.attach(ESCPin1_TT);
  ESC_RB.attach(ESCPin4_RB);
  ESC_LB.attach(ESCPin6_LB);
  ESC_TB.attach(ESCPin3_TB);

  Serial.print( "Initialize ESC... " );
  ESC_RT.write(PWMZero); 
  ESC_LT.write(PWMZero); 
  ESC_TT.write(PWMZero); 
  ESC_RB.write(PWMZero); 
  ESC_LB.write(PWMZero); 
  ESC_TB.write(PWMZero); 

  // Here we zero the ESCs and wait for 20 seconds. You must zero the ESCs to 
  // initialize them. You only need to wait 5 seconds for initialization, but
  // I wait longer, just to get out of the way of the Y6.
  // Also, change the switch in UART position
  for (int count = 20; count >= 1; count--){
    Serial.println(count);
    delay(1000);
  }
  
  // Initializing sensors
  initSensors();
  getSensorData();
  
  // Now we set the minimum PWM value before starting the test.
  
  Serial.println( "initialization complete... " );
  Serial.println( "Starting test... " );
}

boolean ESC_ON = false;

int PWMValue_RT = 10;
int PWMValue_LT = 10;
int PWMValue_TT = 10;
int PWMValue_RB = 18;
int PWMValue_LB = 18;
int PWMValue_TB = 18;

void ascend(){
  PWMValue_RT++;
  PWMValue_LT++;
  PWMValue_TT++;
  PWMValue_RB++;
  PWMValue_LB++;
  PWMValue_TB++;
}

void descend(){
  PWMValue_RT--;
  PWMValue_LT--;
  PWMValue_TT--;
  PWMValue_RB--;
  PWMValue_LB--;
  PWMValue_TB--;
}

void startMission(){
  ESC_ON = true;
}

void abortMission(){
  ESC_ON = false;
}

void log_pwm(){
  Serial.print("PWMValue RT = "); 
  Serial.println(PWMValue_RT);
  Serial.print("PWMValue LT = "); 
  Serial.println(PWMValue_LT);
  Serial.print("PWMValue TT = "); 
  Serial.println(PWMValue_TT);
  Serial.print("PWMValue RB = "); 
  Serial.println(PWMValue_RB);
  Serial.print("PWMValue LB = "); 
  Serial.println(PWMValue_LB);
  Serial.print("PWMValue TB = "); 
  Serial.println(PWMValue_TB);
  getSensorData();
}

void loop() {
  // We are going to use 2 char commands
  if (Serial.available()) {
    char command = Serial.read();
    switch (command){
      case '1': if (PWMValue_RT<PWMMax) PWMValue_RT++; ESC_RT.write(PWMValue_RT); break;
      case '2': if (PWMValue_LT<PWMMax) PWMValue_LT++; ESC_LT.write(PWMValue_LT); break;
      case '3': if (PWMValue_TT<PWMMax) PWMValue_TT++; ESC_TT.write(PWMValue_TT); break;
      case '4': if (PWMValue_RB<PWMMax) PWMValue_RB++; ESC_RB.write(PWMValue_RB); break;
      case '5': if (PWMValue_LB<PWMMax) PWMValue_LB++; ESC_LB.write(PWMValue_LB); break;
      case '6': if (PWMValue_TB<PWMMax) PWMValue_TB++; ESC_TB.write(PWMValue_TB); break;
      case 'q':
      case 'Q': if (PWMValue_RT>PWMMin) PWMValue_RT--; ESC_RT.write(PWMValue_RT); break;
      case 'w':
      case 'W': if (PWMValue_LT>PWMMin) PWMValue_LT--; ESC_LT.write(PWMValue_LT); break;
      case 'e':
      case 'E': if (PWMValue_TT>PWMMin) PWMValue_TT--; ESC_TT.write(PWMValue_TT); break;
      case 'r':
      case 'R': if (PWMValue_RB>PWMMin) PWMValue_RB--; ESC_RB.write(PWMValue_RB); break;
      case 't':
      case 'T': if (PWMValue_LB>PWMMin) PWMValue_LB--; ESC_LB.write(PWMValue_LB); break;
      case 'y':
      case 'Y': if (PWMValue_TB>PWMMin) PWMValue_TB--; ESC_TB.write(PWMValue_TB); break;
      case 'a': 
      case 'A': abortMission(); break;
      case 's': 
      case 'S': startMission(); break;
      case 'u':
      case 'U': ascend();       break;
      case 'd': 
      case 'D': descend();      break;
      case 'l':
      case 'L': log_pwm();      break;
      default:                  break;
    }
  }
  if (ESC_ON) {
    ESC_RT.write(PWMValue_RT);
    ESC_LT.write(PWMValue_LT);
    ESC_TT.write(PWMValue_TT);
    ESC_RB.write(PWMValue_RB);
    ESC_LB.write(PWMValue_LB);
    ESC_TB.write(PWMValue_TB);
  }
  else {
    ESC_RT.write(PWMZero);
    ESC_LT.write(PWMZero);
    ESC_TT.write(PWMZero);
    ESC_RB.write(PWMZero);
    ESC_LB.write(PWMZero);
    ESC_TB.write(PWMZero);
  }
}
