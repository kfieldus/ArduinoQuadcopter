/*
QUADCOPTER CONTOROLLER SKETCH By Kyle Fieldus 

Implemented using an Arduino Uno, Xbee and Funduino Joystick Shield.
Controls defined as follows:

Analog Up / Down = Power Up / Down
Analog Left /Right = Yaw Left / Right
Analog Push = Reset Power to 0
Middle left button (Select Button) = Prime Toggle
Middle right button (Start Button) = Arm Toggle
Directional tactile buttons = Direction changes for Roll and Pitch

Data is transmitted to the Quadcopter using an array of bytes in the following configuration

[Head Bit] - [Status] - [Power] - [Roll] - [Pitch] - [M1 Trim] - [M2 Trim] - [M3 Trim] - [M4 Trin]
*/

int up_button = 2;                                   // Up tactile button attached to pin D2     
int down_button = 4;                                 // Down tactile button attached to pin D4
int left_button = 5;                                 // Left taectile button attached to pin D5
int right_button = 3;                                // Right tactile button attached to pin D3
int start_button = 6;                                // Start button is attached to pin D6
int select_button = 7;                               // Select button is attached to pin D7
int analog_button = 8;                               // Analog Push button is attached to pin D8
int aled =13;                                        // LED for armed status is on pin D13
int pled =11;                                        // LEd for primed status is on pin D12
float x_axis = A0;                                   // Analog X axis attached to pin A0
float y_axis = A1;                                   // Analog Y axis attached to pin A1
// Creating an array of all buttons in this sketch to allow me to access all buttons with few lines of code
int buttons[] = {up_button, down_button, left_button, right_button, start_button, select_button, analog_button};
int  x_map, y_map, analog_button_state;        // Set of gloab int varibales for power and mapped x/y values
char power;
byte out[5];                                         // Byte array for storing the data to be transmitted later
boolean power_clear = false;                         // Flag used to 0 the power readings
boolean firstbyte = true;
int armed = LOW;                                     // Flag used for the armed status
int primed = LOW;                                    // Flag used for the primed status
char ap_status;
int throttle;
char roll;
char pitch;
char yaw;
int redybit;

//debouncing
int start_reading, select_reading;                   // Variables to store initial readings for start/select button states
int start_button_state;                              // Variable for storing the current state of start button
int select_button_state;                             // Variable for storing the current state of the select button
int select_button_last_state = HIGH;                 // Variable for the last known state of the select button
int start_button_last_state = HIGH;                  // Variable for the last known state of the start button
long last_check = 0;                                 // Variable for the last time we checked the start button
long last_select_check = 0;                          // Variable for the last time we checked the select button
long debounceDelay = 200;                            // Delay to ignore button presses

void setup() {
  pinMode(aled, OUTPUT);                             // Enable the Arned LED pin as an output
  pinMode(pled, OUTPUT);                             // Enable the Primed LED pin as an output
  for (int i; i < 7; i++)                            // Loop Through All Buttons 
  {
     pinMode(buttons[i], INPUT);                     // Set each pin named earlier as an Inpuet
     digitalWrite(buttons[i], HIGH);                 // Enable the Pull Up Resistors on Each Button
  } 
  Serial.begin(115200);                              // Start the Serial Communication Bus
}

void loop(){
  checkButtons();                                   // Method used to check all input devices on the shield
  control_status();                                 // Checks for the current status of the controller (armed / primed)
  readData();                                       // Used to read the ready flag from the quadcopter before outputting the data.
}
