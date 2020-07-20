/////////////////////////////////////////////////
//      Method to attach motors in software    //
/////////////////////////////////////////////////
void initialiseMotors() {
  #ifdef DEV_MODE
  Serial.println("Attatching Motors...");
  #endif
  m1.attach(M1_PIN, ESC_ARM, ESC_MAX);                              // Motor 1 is on pin D6
  m2.attach(M2_PIN, ESC_ARM, ESC_MAX);                              // Motor 2 is on pin D5
  m3.attach(M3_PIN, ESC_ARM, ESC_MAX);                              // Motor 3 is on pin D9
  m4.attach(M4_PIN, ESC_ARM, ESC_MAX);                              // Motor 4 is on pin D10
  #ifndef esc_set                                                   // Only if we aren't calibrating the ESCs
  change_speed(ESC_ARM, ESC_ARM, ESC_ARM, ESC_ARM);                 // Send the arming speed to all motors 
  #endif
  #ifdef DEV_MODE                                                   // Only print text if dev flag is set
  Serial.println("Attatching Motors...Done");                       // Print message to show how far we have come
  #endif
}

/////////////////////////////////////////////////
//      Method to configure the ESC range      //
/////////////////////////////////////////////////
void calibrate_esc() {                
  Serial.println("Setting ESC throttle range...");                 // Serial output for user benefit only
  while (!Serial.available() > 0) {                                // While no serial data being received from the User
  change_speed(ESC_MAX, ESC_MAX, ESC_MAX, ESC_MAX);                // Send full throttle signal to ESC's to put them in range setting mode
  Serial.println("HIGH");                                          // Output to use to show progress
  }
  for (int i; i < 10; i++) {                                       // When user sends serial character loop 10 times
  change_speed(ESC_MIN, ESC_MIN, ESC_MIN, ESC_MIN);}               // Send the minimum valid singnal to the ESCs to end the processs.
  Serial.println("FINISHED");                                      // Output the finished message to the serial monitor for the user
}

/////////////////////////////////////////////////
//      Method to apply speeds to motors       //
/////////////////////////////////////////////////
void change_speed(int m1s, int m2s, int m3s, int m4s)              // Receives 4 int values, one for each motor
{                                                                  // Speeds are written to motors in order of (2,3,1,4) to keep balance
  m2.writeMicroseconds(m2s);                                       // Send the speed for motor 2 
  cm2s = m2s;                                                      // Store a copy of the speed sent incase we run into trouble
  m3.writeMicroseconds(m3s + 15);                                  // Send the speed for motor 3 (+15 to compensate for hardware performance difference) 
  cm3s = m3s;                                                      // Store a copy of the speed sent incase we run into trouble
  m1.writeMicroseconds(m1s);                                       // Send the speed for motor 1
  cm1s = m1s;                                                      // Store a copy of the speed sent incase we run into trouble
  m4.writeMicroseconds(m4s);                                       // Send the speed for motor 4
  cm4s = m4s;                                                      // Store a copy of the speed sent incase we run into trouble
  cms_av = ((cm1s+cm2s+cm3s+cm4s)/4);                              // Calculate the average speed over all motors
} 

/////////////////////////////////////////////////
//  Method to start the motors idiling         //
/////////////////////////////////////////////////
void start_motors() {
  while(primed == false)                                       // If armed but not spinning motors do this
  {
    for (int i=ESC_ARM; i<=MOTOR_IDLE; i++)                    // Start at lowest value stop at idle speed 
    {
      led_color(0,255,0);                                      // Change led color to Green to show moving action iminent.
      change_speed(i,i,i,i);                                   // Send the current value to change_speed method
    } 
    primed = true;                                             // Set the flag to true so this doesn't run again. 
  }
}

/////////////////////////////////////////////////
//  Method to act on commands from controller  //
/////////////////////////////////////////////////
void updateMotors() {
  if (armed == false)                                          // If not armed do this
  {    
    change_speed(ESC_ARM,ESC_ARM,ESC_ARM,ESC_ARM);             // Send lowest value to the motors to keep ESCs alive
    led_color(255,0,255);                                      // Keep the system led Purple, the drone will not do anything here
  }

  switch (rcvd[0])                                             // Depending on what value is in slot 1 do the following                     
  {                                                            // Slot 1 is reserved for the armed, primed status sent from the controller
  case '%':                                                    // If this char is received nothing has been pressed on the contorller                         
    led_color(255,0,255);                                      // Keep the LED purple
    change_speed(ESC_ARM,ESC_ARM,ESC_ARM,ESC_ARM);             // Ensure only the lowest valid signal is sent to keep ESC alive. No movement
    power = 0;                                                 // Power is throttle from controller, set to 0. No movement desired
    primed = false;                                            // Keep the primed flag as false
    armed = false;                                             // Keep the armed flag as false
   // updatePID();
    break;                                                     // Break here until next set of data received
  case '$':                                                    // If this char is recived the command to arm has been sent from controller
    led_color(255,0,0);                                        // Change LED to Red to indicate armed status is true
    change_speed(ESC_ARM,ESC_ARM,ESC_ARM,ESC_ARM);             // Only send lowest valid signal to keep ESC alive. No movement
    power = 0;                                                 // Power is throttle from controller, set to 0. No movement desired         
    primed = false;                                            // Keep the primed flag as false
    armed = true;                                              // Set the armed flag to true
   // updatePID();
    break;                                                     // Break here until next set of data received
  case '#':                                                    // If this char received the controller has sent prime (after previous arm) 
    if(primed == false && armed == true)                       // If this is the first time primed has been set motors aren't spinning yet
    {
      led_color(0,255,0);                                      // Change the LED to green
      start_motors();                                          // This method start the motors idling
      updatePID();                                             // Calculate the PID correction values
    } 
    else                                                       // primed is true and motors are assumed to be already spinning                                     
    {
      led_color(0,255,0);                                      // Ensure LED is green
      readMPU();                                               // Get the latest values from teh MPU6050
      updatePID();                                             // Get the latest values from the PID controllers
      switch(rcvd[1])                                          // Slot 2 of array is the throttle value from the controller stick
      {
      case '^':                                                // If this char is received the stick has been clicked in (button action)
        power = 0;                                             // power is 0, clear power to nothing
        break;                                                 // Break at this point and wait for next set of data
      case '*':                                                // If this char is received the stick hasn't been touched, throttle unchanged.
        power = power;                                         // power hasn't changed
        break;                                                 // Break at this point until new data received.
      case '&':                                                // If this char is recived the stick is in the up position, throttle increase
        if (power != MAX_POWER && millis() - last_pwr > 50)    // If power ins't at maximum and gap betwen now and last change is at least 200ms
        {                                                      // Without this the throttle will hit maximum withing a few seconds
          power = power + 1;                                   // Increase the power variable by 0.25                  
          last_pwr = millis();                                 // Set the last_pwr variable to now
        }
        break;                                                 // Break at this point
      case '(':                                                // If this char is receied the stick is in bottom position, throttle decrease
        if(power != MIN_POWER && millis() - last_pwr > 50)     // If power isn't already 0 and gap between now and last change is at least 200ms
        {
          power = power - 1;                                   // Decrease the power varoable by 0.25
          last_pwr = millis();                                 // Set the last_pwr variable now
        }   
        break;                                                 // Break at this point. 
      }

      switch(rcvd[2])                                          // Slot 3 of array is the roll value from the controller buttons
      {//roll
      case '*':                                                // This character represents no change
        ctrl_roll = 0;                                         // Set to 0, as no change.
        break;
      case '&':                                                // This character represents increase requested
        ctrl_roll = 5;                                         // Set to 5, not allowing this to grow higher than 5
        break;
      case '(':                                                // This character represents decrease requested
        ctrl_roll = -5;                                        // Set to -5, not allowing this to drop below -5
        break;  
      }
      switch(rcvd[3])                                           // Slot 4 of array is the pitch value from the controller buttons
      {//pitch
      case '*':                                                 // This character represents no change
        ctrl_pitch = 0;                                         // Set to 0, as no change.
        break;
      case '&':                                                 // This character represents increase requested
        ctrl_pitch = 5;                                         // Set to 5, not allowing this to grow higher than 5
        break;
      case '(':                                                 // This character represents decrease requested
        ctrl_pitch = -5;                                        // Set to -5, not allowing this to drop below -5
        break;  
      }
   switch(rcvd[4])                                           // Slot 4 of array is the yaw value from the controller buttons
      {//pitch
      case '*':                                               // This character represents no change
        ctrl_yaw = 0;                                         // Set to 0, as no change.
        break;
      case '&':                                               // This character represents increase requested
        ctrl_yaw = 5;                                         // Set to 5, not allowing this to grow higher than 5
        break;
      case '(':                                               // This character represents decrease requested
        ctrl_yaw = -5;                                        // Set to -5, not allowing this to drop below -5
       break;  
      }
    }
    break;                                                    // Break here.
  }              
}




