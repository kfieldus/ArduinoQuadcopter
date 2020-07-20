# ArduinoQuadcopter
The Arduino Quadcopter and Controller Code which made up my Quadcopter Project. 

## Whats inside this project?
Project Sketches by Kyle Fieldus - Within this directory are all of the sketch files, created by myself, that make the project. I have grouped the files into folders named after the element of the project they are associated with. (i.e. Controller, Quadcopter, DashboardGUI)

External Libraries Used - These are open source libraries included within this project that were not developed by me. I have grouped them into folders depending on their relationship to this project.

Modified Libraries - I modified a copy of the current Servo library from the Arduino IDE. I renamed the
library to ensure that my changes are not lost after an application update.

Sample Sensor Sketches - These sketches were supplied with the ADXL345 and L3G4200 sensors. The sketches were used to understand the way to interact with the sensors but do not appear within the final program.

TouchOSC Controller Layout - This file is the saved layout template file that would need to be shared to any other mobile device wishing to control the quadcopter. (Requires TouchOSC Editor to be installed)

IDE Installers - Downloaded installers for the Arduino, Processing and TouchOSC Environments

## Installing the sketches and libraries
For reviewing the sketches and project as a whole, I recommend doing the following, to ensure that all dependencies are in place.

## Requirements
- Processing IDE from https://processing.org/download/ 
- Arduino IDE from http://arduino.cc/en/Main/Software 

## Instructions
Once the Arduino IDE is installed, copy the contents of <DISK>/INSTALL/Arduino IDE Library Install/ to the following location ~/Documents/Arduino/Libraries/ (This is the location of the library folder for the current user.

Relaunch the IDE and the Quadcopter and Controller sketches can now be accessed from the Examples menu.
FILE > EXAMPLES > ArduinoQuadcopter > Quadcopter or Controller

Once the Processing IDE is installed, copy the contents of <DISK>INSTALL/Processing IDE Library Install/ to the following location ~/Documents/Processing/Libraries/ (This is the location of the library folder for the current user).

Relaunch the Processing IDE, the dashboard GUI can be access through the examples menu of the IDE.
FILE > EXMAPLES > arduinoQuadcopterGUI > arduinoQuadcopterGUI


