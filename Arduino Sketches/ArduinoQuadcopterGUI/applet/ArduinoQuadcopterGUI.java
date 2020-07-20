import processing.core.*; 
import processing.xml.*; 

import oscP5.*; 
import netP5.*; 
import processing.serial.*; 
import cc.arduino.*; 
import java.net.InetAddress; 
import processing.opengl.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class ArduinoQuadcopterGUI extends PApplet {

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
                        //This library is used to connect to TouchOSC Apps
                        //This library is used to read in the data from TouchOSC Apps
            //This library is used to use Serial communication
                   //This library is used to communicate with an Arduino
           //This library is used to get localhost IP Address

////////////  END OF EXTERNAL LIBRARY IMPORTS /////////////////

////////////  OBJECT DECLARATIONS /////////////////////////////
OscP5 oscP5;                           //Declare an object of type oscP5 to use
Serial xbee;                           //Set xbee as a serial object
InetAddress inet;                      //Declare an object to get the local IP Address
PImage img;                            //Delcaring an object of type image
PImage img2;
PFont f;
PFont f2;

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
long time;
String typed = "";
String port = "";
int port_number = 0;
boolean port_chosen = false;
boolean xbee_connected = true;
////////////  END OF GLOBAL VARIABLE DEFINITIONS //////////////

////////////  SETUP PROCEDURE /////////////////////////////////
public void setup()
{
  frame.setTitle("Arduino Quadcopter - Dashboard");      //Sets the title bar text
  size(800, 600, OPENGL);                                    //The size of the application window
  hint(ENABLE_NATIVE_FONTS);
  background(50);                                    //Light grey background
  smooth();                                          //Graohical smoothing function
  xbee = new Serial(this, Serial.list()[0], 57600); //Define serial port for the Xbee device
  oscP5 = new OscP5(this, 8000);                     //Define oscP5 port to listen for data on
  img = loadImage("trans_drone.gif");                //Image to place onto the dashboard
  img2 = loadImage("trans_drone_tiny.gif");                //Image to place onto the dashboard
  f = createFont("Arial", 12, false);
  f2 = createFont("Arial", 36, false);
  textFont(f);

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

public void draw()
{ 
  fill(0);
  rect(0, 420, 800, 180);
  if (xbee_connected) {
    fill(255);
    rect(300, 460, 50, 25);
  }
  fill(0);                                          //Black background for drawn objects
  stroke(255, 140, 0);                              //Orange outline for drawn objects
  rectMode(CORNER);                                 //Measure xy from top left corner
  rect(200, 99, 200, 200);                          //Position and size of box of accelerometer box
  rect(25, 99, 67, 200);                            //Position and size of throttle slider
  rect(520, 200, 150, 67);                          //Position and size of yaw slider
  ellipse(540, 140, 30, 30);                         //Position and size of Armed LED
  ellipse(655, 140, 30, 30);                         //Position and size of Primed LED
  rect(0, 420, 800, 1);  
  image(img, 575, 460);
  fill(255, 140, 0);                                //Sets the typface colour to yellow/orange
  ///////////////////  TEXT LABELS USED ARE DEFINED HERE ///////////////////////
  textFont(f2);
  text("Arduino Quadcopter - Dashboard", 160, 22, 1000, 250);
  textFont(f);
  text("Throttle", 32, 330, 50, 50);                
  text("Pitch and Roll From Accelerometer", 200, 330, 200, 160); 
  text("-100%", 92, 92, 50, 50);
  text("-50%", 92, 192, 50, 50);
  text("-0%", 92, 292, 50, 50);
  text("By Kyle Fieldus", 590, 580, 300, 300);
  text("Serial port and Communication settings", 32, 400, 300, 300);
  text("Forward", 272, 85, 50, 50);
  text("Right", 405, 192, 50, 50);
  text("Backward", 272, 300, 60, 60);
  text("Left", 171, 192, 50, 50);
  text("Yaw Left", 520, 270, 30, 30);
  text("Yaw Right", 650, 270, 30, 30);
  text("Armed Status", 505, 110, 160, 160);
  text("Primed Status", 620, 110, 160, 160);
  text("Connect mobile device to: ", 32, 580, 160, 25);
  text(myIP, 192, 580, 160, 25);
  if (Serial.list().length < 5)
  {
    fill(255, 0, 0);
    textFont(f2);
    text("No Xbee found, check connection cable", 32, 430, 400, 500);
    xbee_connected = false;
  }
  else {
    text("1 - " + Serial.list()[0], 32, 440, 200, 200);
    text("2 - " + Serial.list()[1], 32, 460, 200, 25);
    text("3 - " + Serial.list()[2], 32, 480, 200, 25);
    text("4 - " + Serial.list()[3], 32, 500, 200, 25);
    text("5 - " + Serial.list()[4], 32, 520, 200, 25);
    text("6 - " + Serial.list()[5], 32, 540, 200, 25);
    xbee_connected = true;
  }
  if (xbee_connected) {
    text("Enter the number of the serial port to use and hit enter", 300, 440, 400, 25);
    fill(0);
    text(typed, 316, 478);
    if (port_chosen) {
      fill(255);
      text("Port " + Serial.list()[port_number] + " is active", 300, 500, 400, 25);
    }
  }
  fill(255);
  text(myIP, 192, 580, 160, 25);
  ///////////////////// END OF TEXT LABELS ////////////////////////////////
  // Basic value smoothing
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


  if (armLed == 0)
  { 
    armed = '%'; //obscure character to signal not armed
    throttle = 0; // Clear the current power
    out[0] = PApplet.parseByte(armed); //send not armed signal
    out[1] = PApplet.parseByte('*'); //send 0's for all other data elements in line
    out[2] = PApplet.parseByte('*');
    out[3] = PApplet.parseByte('*');
    out[4] = PApplet.parseByte('*'); 
    sendData();
  }

  if (armLed != 0 && startupLed == 0) //The Armed button has bee pressed
  {
    armed = '$';
    throttle = 0;
    //Read the data from the iPhone into an array to send as a line of data
    // [armed],[throttle],[xrot],[zrot]
    fill(255, 0, 0);
    stroke(0);
    ellipse(540, 140, 15, 15);
    out[0] = PApplet.parseByte(armed);
    out[1] = PApplet.parseByte('*'); //40 is initating value for ESC's
    out[2] = PApplet.parseByte('*');
    out[3] = PApplet.parseByte('*');
    out[4] = PApplet.parseByte('*'); // n_trim
    sendData();
  }
  else if (startupLed != 0 && armLed !=0) // Primed and Armed are pressed
  {
    armed = '#';
    if (millis() - time > 30)
    {
      switch(throttle_change) {
      case 0:
        throttle = throttle;
        throttle_char = '*';
        break;
      case 1:
        if (throttle != 200) {  
          throttle = throttle + 1;
          throttle_char = '&';
        }
        else {
          throttle = throttle;
          throttle_char = '*';
        }
        break;
      case -1:
        if ( throttle != 0) {
          throttle = throttle - 1;
          throttle_char = '(';
        }
        else {
          throttle = 0 ;
          throttle_char = '*';
        }
        break;
      }
      time = millis();
    }
    switch(yaw_change) {
    case 0:
      yaw_char = '*';
      fill(255, 0, 0);
      stroke(255, 0, 0);
      rect(595, 200, 4, 67);
      break;
    case 1:
      yaw_char = '&';
      fill(255, 0, 0);
      stroke(255, 0, 0);
      rect(665, 200, 4, 67);
      break;
    case -1:
      yaw_char = '(';
      fill(255, 0, 0);
      stroke(255, 0, 0);
      rect(520, 200, 4, 67);
      break;
    }
    // Draw the throttle position bar
    fill(50, 40, 40);
    stroke(255, 0, 0);
    rect(25, 99+200, 67, -(throttle)); //Doubled to make drawn box the same size as accell box

    // Draw the Armed LED
    fill(255, 0, 0);
    stroke(0);
    ellipse(540, 140, 15, 15);

    //Draw the Primed LEd
    fill(0, 255, 0);
    stroke(0);
    ellipse(655, 140, 15, 15);

    //Green Ellipse from Accel 
    fill(0, 255, 0);
    stroke(0); //black outline
    if (zrot<=92 && zrot >=-92 && xrot >=-92 && xrot <=92) {
      //ellipse((300+zrot), (139-xrot), 15, 15);
      image(img2, (273+zrot), (175-xrot));
    }
    else if (zrot>92) {
      //ellipse((392), (139-xrot), 15, 15);
      image(img2, (390), (175-xrot));
    }
    else if (zrot<-92) {
      //ellipse((208), (139-xrot), 15, 15);
      image(img2, (198), (175-xrot));
    }
    else if (xrot<-92) {
      //ellipse((300+zrot), (231), 15, 15);
      image(img2, (273+zrot), (281));
    }
    else if (xrot>92) {
      // ellipse((300+zrot), (47), 15, 15);
      image(img2, (273+zrot), (110));
    }

    if (xrot > 60) {
      pitch_char = '&';
    } 
    else if (xrot < -60 ) {
      pitch_char = '(';
    } 
    else {
      pitch_char = '*';
    }

    if (zrot > 60) {
      roll_char = '&';
    } 
    else if (zrot < -60 ) {
      roll_char = '(';
    } 
    else {
      roll_char = '*';
    }

    out[0] = PApplet.parseByte(armed);
    out[1] = PApplet.parseByte(throttle_char); 
    out[2] = PApplet.parseByte(pitch_char);
    out[3] = PApplet.parseByte(roll_char);
    out[4] = PApplet.parseByte(yaw_char);
    sendData();
  }
}

// Method used to send the data in bytes to the Xbee
public void sendData()
{
  xbee.write((byte)'!');
  xbee.write(out[0]); // send to the Arduino over Serial.
  xbee.write(out[1]); // send to the Arduino over Serial.
  xbee.write(out[2]); // send to the Arduino over Serial.
  xbee.write(out[3]); // send to the Arduino over Serial.
  xbee.write(out[4]); // send to the Arduino over Serial.
}

// Method used to read in the information from the touchOScx
public void oscEvent(OscMessage theOscMessage)
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
    armLed = PApplet.parseInt(theOscMessage.get(0).floatValue()); //store the value;
  }

  if (addr.equals("/2/prime"))//if statupbutton
  {
    startupLed = PApplet.parseInt(theOscMessage.get(0).floatValue());
  }
}

public void keyPressed() {
  if (!port_chosen) {
    if (key == '\n') {
      port_number = Integer.parseInt(typed) -1; 
      set_serialPort(port_number);
      port_chosen = true;
    } 
    else {
      typed = typed + key;
    }
  }
}

public void set_serialPort(int selection) {
  xbee.stop();
  xbee = new Serial(this, Serial.list()[selection], 57600);
}

  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#FFFFFF", "ArduinoQuadcopterGUI" });
  }
}
