//  This method sets the first two bytes in the data array depending on armed and primed status
void control_status()                              
{
  if (armed == LOW && primed == LOW)              // Not Armed and Not Primed
  {
   ap_status = '%';                            // Send this character to signal unarmed and not primed
  }
  
  if (armed == HIGH && primed == LOW)             // Armed but not primed.
  {
   ap_status = '$';                            // Send this chracter to signla armed but not primed
  }
  
  if (armed == LOW && primed == HIGH)             // Not Armed and Primed, Shouldn't ever get here
  {
   ap_status = '%';                            // Send the same as not armed and not primed
  }
  
  if (armed == HIGH && primed == HIGH)            // Armed and Primed
  {
   ap_status = '#';                            // Send this character to show armed and primed are true
  }   
}

// This method is used to modify the current power value. This receives either a +1 or -1 as its argument
void change_power(int y)
{
  switch(y)                                   // Switch on the value passed
  {
     case 255:                                // If 255 received the thumb stick has been clicked  
        power = '^';                          // Send char
        power_clear = false;                  // Clear the power build up
        break;
     case 1:                                  // If 1 
        power = '&';                          // Send the increment char
        break;          
     case 0:                                  // If 0
        power = '*';                          // Send the no change char
        break;  
     case -1:                                 // if -1
        power = '(';                          // Send the decrease char
        break;   
  }
  
}

// This method is used to modify the current pitch value. Receives wither +1, -1 or 0 and selects the appropriate char.
void change_pitch(int p) {
  switch(p)
  {
     case 1:                                  // If 1 
        pitch = '&';                          // Send the increment char
        break;          
     case 0:                                  // If 0
        pitch = '*';                          // Send the no change char
        break;  
     case -1:                                 // if -1
        pitch = '(';                          // Send the decrease char
        break;  
  }
}

// This method is used to modify the current roll value. Receives wither +1, -1 or 0 and selects the appropriate char.
void change_roll(int r) {
  switch(r)
  {
     case 1:                                  // If 1 
        roll = '&';                          // Send the increment char
        break;          
     case 0:                                  // If 0
        roll = '*';                          // Send the no change char
        break;  
     case -1:                                 // if -1
        roll = '(';                          // Send the decrease char
        break;  
  }  
}

// This method is used to modify the current pitch yaw. Receives wither +1, -1 or 0 and selects the appropriate char.
void change_yaw(int yw) {
switch(yw)
  {
     case 1:                                  // If 1 
        yaw = '&';                          // Send the increment char
        break;          
     case 0:                                  // If 0
        yaw = '*';                          // Send the no change char
        break;  
     case -1:                                 // if -1
        yaw = '(';                          // Send the decrease char
        break;   
  }    
}
