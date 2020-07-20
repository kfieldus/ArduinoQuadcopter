void checkButtons(){
  start_reading = digitalRead(start_button);       // Read the current value of the start button and store it
  select_reading = digitalRead(select_button);     // Read the current value of the select button and store it

  // If the reading is not the same as bfore and the time since last check is more than the allowed time
  if (start_reading != start_button_last_state && (millis() - last_check) > debounceDelay)
  {
    armed = !armed;                                 // Toggle the armed status (low -> high or high -> low)
    set_armed(armed);                               // Pass the current armed status to the set_armed method
    last_check= millis();                           // Store the time at this point to be used in next check
  }   

  // If the status is armed and select button status is different and also the time since last check is more than alllowed time
  if (armed == HIGH &&  select_reading != select_button_last_state && (millis() - last_select_check) >debounceDelay)
  {
    primed = !primed;                               // Toggle the primed status (low -> high or high -> low)
    set_primed(primed);                             // Pass the current primed status to the set_primed method
    last_select_check = millis();                   // Store the time at this poing to be used in next check.
  }

  // If the armed and primed status' are true then and only then send increased throttle values and other modifiers
  if (armed == HIGH && primed == HIGH)              
  {
    x_map = map(analogRead(x_axis), 0, 500, -1, 1);   // Read the X value and map it to a value between -1 and +1 
    y_map = map(analogRead(y_axis), 0, 500, -1, 1);    // Read the Y value and map it to a value between -1 and +1
    analog_button_state = digitalRead(analog_button);   // Read the Analog button state

    if (analog_button_state == HIGH)               // If the analog button isn't pressed     
    {
      power_clear = false;                         // Do not reset the power level to 0
      change_power(y_map);                         // Send the current power value to the change_power method
      change_yaw(x_map);  
    }

    if (analog_button_state == LOW)                // If the analog button is pressed
    {
      power_clear = true;                           // Set the reset power floag to true
      change_power(255);                              // Send the value 0 to the change_speed method
    }
  }
  else                                              // Anything at this stage is an error
  {
    power_clear = true;                              // Set the reset power flog to be true
    change_power(255);                                 // Send teh value 0 to the change_speed method
  }



  if (digitalRead(up_button) == LOW)                // If the up button has been pressed
  {
    change_pitch(1);
  }                                                 // Send +1 to the method change_pitch
  else if (digitalRead(down_button) == LOW)        // If the down button has been pressed
  {
    change_pitch(-1);                               // Send -1 to the method change_pitch
  }                                                 
  else {
    change_pitch(0);                                // Anything else should be considered as no change.
  }                            

  if (digitalRead(left_button) == LOW)              // If the left button has been pressed
  {
    change_roll(-1);
  }                                                  // Send a positive value to the change_roll method
  else if (digitalRead(right_button) == LOW)        // If the right button has been pressed
  {
    change_roll(1);                                 // send a negative value to the method change_roll
  }                                                 
  else 
  {  
    change_roll(0);                                 // Anything else at this point should be considered as no change.
  }                                  


  delay(150); 
}

