/////////////////////////////////////////////////
//  Safety power down routine if comms lost    //
/////////////////////////////////////////////////
void watchdog() {
  led_color(0,0,255);                              // Set the onboard LED to Blue
  while (no_comm == true)                          // While the no signal flag is true to this
  {
    power = MIN_POWER;                            // Clear the additional throttle flag

  
    if (millis() - wtchdg > 500)                   // To ensure throttle down is smooth and slow using a timer delay
    {
      cm1s--;                                      // Decrease the speed value of motor 1 by 1
      cm2s--;                                      // Decrease the speed value of motor 2 by 1
      cm3s--;                                      // Decrease the speed value of motor 3 by 1
      cm4s--;                                      // Decrease the speed value of motor 4 by 1
      change_speed(cm1s, cm2s, cm3s, cm4s);        // Send the speeds for all motors to the motors themselves. 
      wtchdg = millis();                           // Set the wtchdg variable to now
    }
    if (cms_av <= ESC_ARM)                              // If the average speed of all motors is less or equal to lowest value
    {
      primed, armed = false;                     // Reset the armed, primed and no_comm flags to false to stop random power ups if connection is re-established.
      stay_off = true;                           // Ensure the motors never spin again during this session, set this flag to true.
    }                                            
  } 
  led_color(0,150,255);                         // Set the onboard LED colour to light blue
  //while (stay_off = true){                      // While the stay off flag is true
  //change_speed(0,0,0,0);                        // send 0 to all motors for safety.
 // }
}

