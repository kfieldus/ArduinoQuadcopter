/* K.W.A.D Copter - Dashboard - By Kyle Fieldus
 
 This application connects with a touchOSC app (Android or iOS), the app sends 
 control information to the local computer via the local wireless network. This 
 dashboard reads in the information received, displays it within a GUI and then 
 sends the data onto an Xbee wireless radio to transmit to the drone.
 
 -- Requirements --
 * iOS or Android device on the same wireless nework as computer running this application
 * TouchOSC client must be installed on the mobile device
 * The KWADCopter touschOSC layout has been loaded onto the device
 * The device must be configured to point to the IP Address shown on the dashbaord window
 * TouchOSC on the mobile must be configured to send the accelerometer data
 */

////////////  EXTERNAL LIBRARY IMPORTS ///////////////////////
import oscP5.*;                        //This library is used to connect to TouchOSC Apps
import netP5.*;                        //This library is used to read in the data from TouchOSC Apps
import processing.serial.*;            //This library is used to use Serial communication
import cc.arduino.*;                   //This library is used to communicate with an Arduino
import java.net.InetAddress;           //This library is used to get localhost IP Address
import processing.opengl.*;            //This library is used to enable OpenGL support for the drawing of objects.
////////////  END OF EXTERNAL LIBRARY IMPORTS /////////////////

////////////  OBJECT DECLARATIONS /////////////////////////////
OscP5 oscP5;                           //Declare an object of type oscP5 to use
Serial xbee;                           //Set xbee as a serial object
InetAddress inet;                      //Declare an object to get the local IP Address
PImage img;                            //Delcaring an object of type image, this will be the large logo
PImage img2;                           // Delcaring an object of type image, this will be the acceleromter cursor                    
PFont f;                               // Creating a Font object
PFont f2;                              // Creating a second Font object.

////////////  END OF OBJECT DECLARATIONS ////////////////////////

////////////  GLOBAL VARIABLE DEFINITIONS ///////////////////////
int throttle = 0;                      //Holds throttle value (local use only, not transmitted) 
int throttle_change = 0;               //Holds the throttle change value from controller (+1, -1, 0)
int yaw_change = 0;                    //Holds the yaw change value from the controller (+1, -1, 0)
float xrot = 0;                        //Holds the X value from the mobile device acceleromter
float zrot = 0;                        //Holds the Z value from teh mobile device acceleromter 
float xrot_targ = 0;                   //Target for accelerometer X value
float zrot_targ = 0;                   //Target for accelerometer Z value
float orientation = 0;                 //Holds the value returned for orientation
float dampSpeed = 50;                  //Changing this value affects how responsive the accelerometer data is
int armLed = 0;                        //Status check for armed (1 = true, 0 = false)
int startupLed = 0;                    //Status check for primed (1 = true, 0 = false) 
char armed;                            //Character to send for armed status ('%'= safe, '$' armed, '#'= amred and primed) 
char throttle_char;                    //Character to send for throttle change ('*' = 0, '&' = +1 '(' = -1)
char pitch_char;                       //Character to send for pitch change ('*' = 0, '&' = +1 '(' = -1)
char roll_char;                        //Character to send for roll change ('*' = 0, '&' = +1 '(' = -1)
char yaw_char;                         //Character to send for yaw change ('*' = 0, '&' = +1 '(' = -1)
byte out[] = new byte[5];              //Array to hold a set of data values before transmission
String myIP;                           //Stores the IP Address of the local network connection
long time;                             //Stores the current system time
String typed = "";                     // Container for the value typed by the user for the serial selection window.
String port = "";                      
int port_number = 0;                    // Port number typed by the user after converting from a String
boolean port_chosen = false;             //Flag for status of chosen Serial port, stops repeated port configurations
boolean xbee_connected = true;           //Flag to signal if an Xbee device is connected
////////////  END OF GLOBAL VARIABLE DEFINITIONS //////////////

////////////  SETUP PROCEDURE /////////////////////////////////
void setup()
{
  frame.setTitle("Arduino Quadcopter - Dashboard");      //Sets the title bar text
  size(800, 600, OPENGL);                                    //The size of the application window
  hint(ENABLE_NATIVE_FONTS);                          //Font substitution routine if required
  background(50);                                    //Light grey background
  smooth();                                          //Graohical smoothing function
  xbee = new Serial(this, Serial.list()[0], 115200); //Define serial port for the Xbee device
  oscP5 = new OscP5(this, 8000);                     //Define oscP5 port to listen for data on
  img = loadImage("trans_drone.gif");                //Image to place onto the dashboard
  img2 = loadImage("trans_drone_tiny.gif");                //Image to place onto the dashboard
  f = createFont("Arial", 12, false);                      // Main font used is type face Arial, point side 12
  f2 = createFont("Arial", 36, false);                     // Heading font is also Arial with point size 36
  textFont(f);                                              // Set the current font to use to be type f and not f2

  ///////////////  CAPTURING THE IP ADDRESS ////////////////////
  // from http://forum.processing.org/one/topic/get-the-ip-address-of-the-host-computer.html
  try 
  {
    inet = InetAddress.getLocalHost();               //Set the object inet to the localhost 
    myIP = inet.getHostAddress();                    //Set the variable myIP to the returned IP Value
  }

  catch (Exception e)                                //If we cannot find the IP Address to this
  {
    e.printStackTrace();                             //Trace how far we came
    myIP = "Couldn't get IP";                        //Output a message to the screen for the user
  }
  ///////////////  END OF CAPTURING THE IP ADDRESS /////////////
}
////////////  END OF SETUP PROCEDURE ///////////////////////////

///////////// MAIN APPLICATION LOOP ////////////////////////////

void draw()
{ 
  fill(0);                                          // Black background
  rect(0, 420, 800, 180);                           // Draws the setial port window at the bottom
  if (xbee_connected) {                             // If an Xbee is detected
    fill(255);                                      // White background (for the next shape)
    rect(300, 460, 50, 25);                        // Draw the text box for the user to select a serial port
  }
  fill(0);                                          //Black background for drawn objects
  stroke(255, 140, 0);                              //Orange outline for drawn objects
  rectMode(CORNER);                                 //Measure xy from top left corner
  rect(200, 99, 200, 200);                          //Position and size of box of accelerometer box
  rect(25, 99, 67, 200);                            //Position and size of throttle slider
  rect(520, 200, 150, 67);                          //Position and size of yaw slider
  ellipse(540, 140, 30, 30);                         //Position and size of Armed LED
  ellipse(655, 140, 30, 30);                         //Position and size of Primed LED
  rect(0, 420, 800, 1);                              // Drawing a horizontal line
  image(img, 575, 460);                              // Placing the larger quadcopter icon on the window
  fill(255, 140, 0);                                //Sets the typface colour to yellow/orange
  ///////////////////  TEXT LABELS USED ARE DEFINED HERE ///////////////////////
  textFont(f2);                                      // Set the font to f2
  text("Arduino Quadcopter - Dashboard", 160, 22, 1000, 250);    // Set the window title heading
  textFont(f);                                          // Change font back to f
  text("Throttle", 32, 330, 50, 50);                    // Label for throttle box
  text("Pitch and Roll From Accelerometer", 200, 330, 200, 160);  // Label for box showing pitch and roll data
  text("-100%", 92, 92, 50, 50);                        // label for throttle box
  text("-50%", 92, 192, 50, 50);                        // label for throttle box
  text("-0%", 92, 292, 50, 50);                        // label for throttle box
  text("By Kyle Fieldus", 590, 580, 300, 300);          // Name placing
  text("Serial port and Communication settings", 32, 400, 300, 300); // title for serial port selection box
  text("Forward", 272, 85, 50, 50);                         // label for pitch and roll box                   
  text("Right", 405, 192, 50, 50);                         // label for pitch and roll box                   
  text("Backward", 272, 300, 60, 60);                      // label for pitch and roll box                   
  text("Left", 171, 192, 50, 50);                         // label for pitch and roll box                   
  text("Yaw Left", 520, 270, 30, 30);                    // label for yaw box                   
  text("Yaw Right", 650, 270, 30, 30);                    // label for yaw box                   
  text("Armed Status", 505, 110, 160, 160);              // label for armed LED,                 
  text("Primed Status", 620, 110, 160, 160);              // label for primed LED, 
  text("Connect mobile device to: ", 32, 580, 160, 25);   // Label for serial port selection window
  text(myIP, 192, 580, 160, 25);                          // Writing the IP Addresss of the computer
  if (Serial.list().length < 5)                            // If there are less than 5 serial devices
  {
    fill(255, 0, 0);                                       // Select the colour red
    textFont(f2);                                          // Set the large font
    text("No Xbee found, check connection cable", 32, 430, 400, 500); // Type the warning message
    xbee_connected = false;                                // NO Xbee conected, set flag to false.
  }
  else {
    text("1 - " + Serial.list()[0], 32, 440, 200, 200);    // Write the name of a detected serial device.
    text("2 - " + Serial.list()[1], 32, 460, 200, 25);    // Write the name of a detected serial device.
    text("3 - " + Serial.list()[2], 32, 480, 200, 25);    // Write the name of a detected serial device.
    text("4 - " + Serial.list()[3], 32, 500, 200, 25);    // Write the name of a detected serial device.
    text("5 - " + Serial.list()[4], 32, 520, 200, 25);    // Write the name of a detected serial device.
    text("6 - " + Serial.list()[5], 32, 540, 200, 25);    // Write the name of a detected serial device.
    xbee_connected = true;                                // Devices detected, set flag to true.
  }
  if (xbee_connected) {
    text("Enter the number of the serial port to use and hit enter", 300, 440, 400, 25); // If true ask the user to select the device
    fill(0);                                               // Select the colour black
    text(typed, 316, 478);                                 // Draw the typed character into the text box     
    if (port_chosen) {                                     // If the port is chosen
      fill(255);
      text("Port " + Serial.list()[port_number] + " is active", 300, 500, 400, 25); // COnfirm the chosen port to the screen
    }
  }
  fill(255);                                              // Selec the colour white
  text(myIP, 192, 580, 160, 25);                          // Draw the IP Address of the workstation.
  ///////////////////// END OF TEXT LABELS ////////////////////////////////
  // Cpturing the acceleromter data from the mobile device acceleromter
  // the damping speed is used to slow the rate of change of the values
  // The x and z axis are used as the mobile device will be held horizontal
  
  if (xrot_targ > xrot)
  {
    xrot = xrot + ((xrot_targ - xrot) / dampSpeed);
  } 
  else 
  {
    xrot = xrot - ((xrot - xrot_targ) / dampSpeed);
  }

  if (zrot_targ > zrot)
  {
    zrot = zrot + ((zrot_targ - zrot) / dampSpeed);
  } 
  else
  {
    zrot = zrot - ((zrot - zrot_targ) / dampSpeed);
  }


  if (armLed == 0)        // If the system is not armed.
  { 
    armed = '%';         //obscure character to signal not armed
    throttle = 0;        // Clear the current power
    out[0] = byte(armed); //send not armed signal
    out[1] = byte('*');  //send 0's for all other data elements in line
    out[2] = byte('*'); //send 0's for all other data elements in line 
    out[3] = byte('*'); //send 0's for all other data elements in line 
    out[4] = byte('*');  //send 0's for all other data elements in line 
    sendData();           // Send the data out
  }

  if (armLed != 0 && startupLed == 0) //The Armed button has bee pressed
  {
    armed = '$';
    throttle = 0;
    //Read the data from the mobile device into an array to send as a line of data
    // [armed],[throttle],[xrot],[zrot]
    fill(255, 0, 0);      // Select the colour red
    stroke(0);            // Set the drawing stroke to black
    ellipse(540, 140, 15, 15); // Draw the circle for the armed LEd
    out[0] = byte(armed);  // Otput the armed character
    out[1] = byte('*'); //send 0's for all other data elements in line 
    out[2] = byte('*'); //send 0's for all other data elements in line 
    out[3] = byte('*'); //send 0's for all other data elements in line 
    out[4] = byte('*'); //send 0's for all other data elements in line 
    sendData();         // Send the data out
  }
  else if (startupLed != 0 && armLed !=0) // Primed and Armed are pressed
  {
    armed = '#';                    // Send the system is both armed and primed signals
    if (millis() - time > 30)       // If more than 30 milliseconds has passed
    {
      switch(throttle_change) {    // Throttle is allowed to change
      case 0:                      // IF 0
        throttle = throttle;       // no change to throttle
        throttle_char = '*';       // send the no change character
        break;
      case 1:                      // IF 1
        if (throttle != 200) {     // If throttle is not at the maximum of 200
          throttle = throttle + 1; // Throttle value increases
          throttle_char = '&';      // Send the increase character
        }
        else {                      // Else do not change throttle
          throttle = throttle;      // No change 
          throttle_char = '*';      // Send 0 change character
        }
        break;
      case -1:                      // IF -1
        if ( throttle != 0) {        // If throttle is not 0 already
          throttle = throttle - 1;   // decrease the local variable of throttle
          throttle_char = '(';        // Send the decrease character
        }
        else {                         //else throttle will remain at 0 
          throttle = 0 ;                // throttle is 0
          throttle_char = '*';          // Send the 0 change chracter
        }
        break;
      }
      time = millis();              // Reset the timer 
    }
    switch(yaw_change) {            // Change in yaw
    case 0:                        // If no change
      yaw_char = '*';              // Send 0 character
      fill(255, 0, 0);              // Set the colour to red
      stroke(255, 0, 0);            // With a red outline
      rect(595, 200, 4, 67);        // draw a light red rectangle showing no change in the direction of yaw control  
      break;
    case 1:                        // IF 1
      yaw_char = '&';              // Send YAW increase char
      fill(255, 0, 0);              // Set the colour to red
      stroke(255, 0, 0);            // Set the stroke to red
      rect(665, 200, 4, 67);        // Draw a light red rectabgle showing a change in the direction of YAW
      break;
    case -1:                        // If -1
      yaw_char = '(';              // send the decrease character
      fill(255, 0, 0);              // set the colour to red
      stroke(255, 0, 0);             // Set the stroke colour to red
      rect(520, 200, 4, 67);          // Draw a light red rectangle showing a change in the direction of YAW
      break;
    }
    // Draw the throttle position bar
    fill(50, 40, 40);              // light red colour
    stroke(255, 0, 0);            // Red outline
    rect(25, 99+200, 67, -(throttle)); //Doubled to make drawn box the same size as accell box

    // Draw the Armed LED
    fill(255, 0, 0); //red
    stroke(0);
    ellipse(540, 140, 15, 15);

    //Draw the Primed LEd
    fill(0, 255, 0); //green
    stroke(0);
    ellipse(655, 140, 15, 15);

    //Draw the cursor for the accelerometer data
    fill(0, 255, 0);                        // Chose a green colour, just incase the image doesn't appear we have a green dot
    stroke(0); //black outline
    if (zrot<=92 && zrot >=-92 && xrot >=-92 && xrot <=92) { // If the data is within the bounds of the box
      image(img2, (273+zrot), (175-xrot));        // draw a small version of the icon in the centre of the box less the accelerated values
    }
    else if (zrot>92) {                          // Explicitly not allowing the image to exceed the bounds of the on screen box
      image(img2, (390), (175-xrot));
    }
    else if (zrot<-92) {                          // Explicitly not allowing the image to exceed the bounds of the on screen box
      image(img2, (198), (175-xrot));
    }
    else if (xrot<-92) {                          // Explicitly not allowing the image to exceed the bounds of the on screen box
      image(img2, (273+zrot), (281));
    }
    else if (xrot>92) {                          // Explicitly not allowing the image to exceed the bounds of the on screen box
      image(img2, (273+zrot), (110));
    }

    if (xrot > 60) {                            // If at this point on the screen
      pitch_char = '&';                        // Send an incrase character for pitch
    } 
    else if (xrot < -60 ) {                    // If at this point on the screen
      pitch_char = '(';                      // Send a decrease character for pitch
    } 
    else {
      pitch_char = '*';                        // Else no change to pitch
    }

    if (zrot > 60) {                          // If at this point on the screen
      roll_char = '&';                        // Send an increase for roll
    } 
    else if (zrot < -60 ) {                    // If at this point on the screen
      roll_char = '(';                          // Send a decrease for roll
    } 
    else {                                
      roll_char = '*';                        // Else roll is unchanged.
    }

    out[0] = byte(armed);                    // Save the characters to the out array
    out[1] = byte(throttle_char);                     // Save the characters to the out array
    out[2] = byte(pitch_char);                    // Save the characters to the out array
    out[3] = byte(roll_char);                    // Save the characters to the out array
    out[4] = byte(yaw_char);                    // Save the characters to the out array
    sendData();                              // Invoke the sendData() method
  }
}

// Method used to send the data in bytes to the Xbee
void sendData()
{
  xbee.write((byte)'!');
  xbee.write(out[0]); // send to the Arduino over Serial.
  xbee.write(out[1]); // send to the Arduino over Serial.
  xbee.write(out[2]); // send to the Arduino over Serial.
  xbee.write(out[3]); // send to the Arduino over Serial.
  xbee.write(out[4]); // send to the Arduino over Serial.
}

// Method used to read in the information from the touchOScx
void oscEvent(OscMessage theOscMessage)
{
  String addr = theOscMessage.addrPattern();           // store pattern received
  float val = theOscMessage.get(0).floatValue();       // store value received

  if (theOscMessage.checkAddrPattern("/accxyz")==true) // if accelerometer
  {
    xrot_targ = (theOscMessage.get(0).floatValue()*90)*-1;   //x value
    zrot_targ = (theOscMessage.get(1).floatValue()*90);      //z value
    //orientation = theOscMessage.get(2).floatValue();         //check if upside down
  }

  if (addr.equals("/2/throttle")) // if throttle control
  {
    //throttle = throttle + (int)val; //store value as an integer
    throttle_change = (int)val;
  } 

  if (addr.equals("/2/yaw")) // if throttle control
  {
    yaw_change = (int)val;
  } 

  if (addr.equals("/2/arm")) //if armed button
  {
    armLed = int(theOscMessage.get(0).floatValue()); //store the value;
  }

  if (addr.equals("/2/prime"))//if statupbutton
  {
    startupLed = int(theOscMessage.get(0).floatValue());
  }
}

void keyPressed() {                            // If a keyboard event has been detected
  if (!port_chosen) {                          // If no port has been chosen yet
    if (key == '\n') {                        // Wait for the return key press
      port_number = Integer.parseInt(typed) -1; // Set the port number to the integer value of the entered string, minus 1 for the array location
      set_serialPort(port_number);              // invoke the set serial port method
      port_chosen = true;                        // Set flag to true so we dont do this again
    } 
    else {
      typed = typed + key;                      // else draw the value typed
    }
  }
}

void set_serialPort(int selection) {          // Method used to enable the selected serial port
  xbee.stop();                                // Stop the current serial port
  xbee = new Serial(this, Serial.list()[selection], 115200); // start the new port with the full baud rate.
}

