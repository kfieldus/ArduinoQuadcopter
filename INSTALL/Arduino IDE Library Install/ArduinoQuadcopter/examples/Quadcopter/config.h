
/////////////////////////////////////////////////
// Instances of Servos and associated variables//
/////////////////////////////////////////////////
#define DEV_MODE                              // Uncomment this flag to enable serial output to terminal
//#define DISABLE_LED                         // Uncomment this to enable the RGB LED
//#define ESC_CALIBRATE                       // Uncomment this to Set the throttle range on ESCs
  
#define ESC_ARM 1023                            // The arming value of the ESCs
#define ESC_MAX 1460                            // The maximum signal that can be sent to the ESCs
#define ESC_MIN 0                               // The minimum signal that can be sent to the ESCs
#define MOTOR_IDLE 1200                         // The value to send to set the EScs to idle the motors
#define MAX_POWER 200                           // The maximum throttle value to add from the controller
#define MIN_POWER 0                             // The absolute minum value of throttle from the controller

#define M1_PIN 6                               // The pin motor 1 is attached to
#define M2_PIN 5                               // The pin motor 2 is attached to
#define M3_PIN 9                               // The pin motor 3 is attached to
#define M4_PIN 10                              // The pin motor 4 is attached to

#define PITCH_P 3                             // Proportional correction value for Pitch axis
#define PITCH_I 7                             // Integral correction value for Pitch axis
#define PITCH_D 0.1                           // Derivative correction value for Pitch axis
#define PITCH_SP 0                            // The Setpoint goal for the Pitch PID

#define ROLL_P 3                              // Proportional correction value for Roll axis
#define ROLL_I 7                              // Integral correction value for Roll axis
#define ROLL_D 0.1                            // Derivative correction value for Roll axis
#define ROLL_SP 0                             // The Setpoint goal for the Roll PID

#define YAW_P 0                               // Proportional correction value for Yaw axis
#define YAW_I 0                               // Integral correction value for Yaw axis
#define YAW_D 0                               // Derivative correction value for Yaw axis
#define YAW_SP 0                              // The Setpoint goal for the Yaw PID

/////////////////////////////////////////////////
/// Connections to LED Pins. Using RGB CC LED///
/////////////////////////////////////////////////
#define red A0                                // The analog pin that connects to the Red leg of RGB LED
#define green A2                              // The analog pin that connects to the Green leg of RGB LED
#define blue A1                               // The analog pin that connects to the Blue leg of RGB LED
