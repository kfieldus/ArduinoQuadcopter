
////////////////////////////////////////////////////////////////////////////////
// External Libraries, developed by other people and used within this project.//
////////////////////////////////////////////////////////////////////////////////
#include "I2Cdev.h"                            // Library used for i2c communication to MPU6050 (Open source library from Jeff Rowberg I2cdevlib)
#include "MPU6050_6Axis_MotionApps20.h"        // Library for complex DMP usage on MPU6050 (Open source library from Jeff Rowberg I2cdevlib)
#include "PIDCont.h"                           // Library ysed for the PID control loops to keep the quadcopter stable. (Open source library by baselsw)
#include "Wire.h"                              // Library used with i2c to use mpu6050 (provided by Arduino IDE)
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// Libraries and other external files, created or modified by Kyle Fieldus for this project.//
//////////////////////////////////////////////////////////////////////////////////////////////
#include "Config.h"                            // This file contains all of the customisable settings
#include "MultirotorServo.h"                   // Library used to control servos using PPM (Modified version of Servo.h from Arduino IDE)
//////////////////////////////////////////////////////////////////////////////////////////////

MultirotorServo m1, m2, m3, m4;               // defines four motors as servos
PIDCont PIDroll, PIDpitch, PIDyaw;            // defines a PID control loop for each axis
double cm1s, cm2s, cm3s, cm4s;                // Used to store the current speed for each motor
double power, last_power;                     // Container for current and last power sent
double cms_av;                                // Variable to hold average motor speeds from drone
double ctrl_yaw, ctrl_pitch, ctrl_roll;       // Used to store the values for yaw pitch and roll from controller

/////////////////////////////////////////////////
/// Variables used within Serial Communication///
/////////////////////////////////////////////////
byte rcvd[5];                                 // Used to store the incoming data
byte last[5];                                 // Used to store the last set of data for comparision
boolean armed, primed, no_comm, stay_off = false;       // Used to allow methods to run once depending on state
long started, waited, last_pwr, last_read, wtchdg;   // Used to time comparisons

/////////////////////////////////////////////////
//  Instance of MPU and associated variables  ///
/////////////////////////////////////////////////
MPU6050 mpu;                                  // Declaring an instance of MPU6050 called mpu
bool dmpReady = false;                        // Set true if DMP init was successful
bool mpu_bias_calculated = false;             // Flag to show if we have calculated an axis bias
uint8_t mpuIntStatus;                         // holds actual interrupt status byte from MPU
uint8_t devStatus;                            // return status after each device operation (0 = success, !0 = error) 
uint16_t packetSize;                          // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;                           // count of all bytes currently in FIFO
uint8_t fifoBuffer[64];                       // FIFO storage buffer
Quaternion q;                                 // [w, x, y, z]         quaternion container
VectorFloat gravity;                          // [x, y, z]            gravity vector
volatile bool mpuInterrupt = false;           // indicates whether MPU interrupt pin has gone high
float ypr[3];                                 // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
int yb, pb, rb;                               // Varoables for Yaw pitch and roll sensor bias
int yrb, prb, rrb;                            // Varoables for Yaw pitch and roll sensor bias
double pitchdir, rolldir, yawdir;
double pitchgyro, rollgyro, yawgyro;

/////////////////////////////////////////////////
//  Instances of PID cntrollers and variables  //
/////////////////////////////////////////////////
double YAW_OUT, PITCH_OUT, ROLL_OUT;          // Output variables for the PID controllers (Yaw, Pitch and Roll)
int m1_pid, m2_pid, m3_pid, m4_pid;           // Values to store each motors speed after PID calculations
/* ## END OF VARIABLE DEFINITIONS ## */

/////////////////////////////////////////////////
//  System setup proceedure to start services  //
/////////////////////////////////////////////////
void setup() {
  led_color(255,255,255);                    // LED is white for setup process  
  Serial.begin(115200);                      // Start Serial bus at 57600 Baud Rate
  initialiseMotors();                        // This method attaches the motor instances in software
  #ifdef ESC_CALIBRATE                       // Only run this if the ESC_CALIBRATE flag is uncommented
  calibrate_esc();                           // This method sets the ESC throttle ranges
  #endif
  initialiseMPU();                           // Configure the DMP for use
  initialisePID();                           // Method to configure the PID control loops
  led_color(255,0,255);                      // All done change the LED to purple    
  #ifdef DEV_MODE                            // Only run this if the DEV flag is uncommented
  Serial.println("Initial Setup...Done");    // Print this message to the terminal screen
  #endif
}

/////////////////////////////////////////////////
//       Main program loop function            //
/////////////////////////////////////////////////
void loop() {
  while(!mpuInterrupt && fifoCount < packetSize){ // Do nothing until data ready on DMP (Short Wait)    
  }
  readData();                                // Get the data coming in over the wireless
  readMPU();                                 // Keep the IMU readings coming even if not armed.
  updatePID();                               // Get the latest PID values
  updateMotors();                            // Based on information we have update the motors
  #ifdef DEV_MODE                            // Only run this if the DEV flag is uncommented
  dev_output();                              // This method outputs an overal system read out to the terminal
  #endif
}




