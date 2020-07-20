/////////////////////////////////////////////////
//  Method to read and store controller data   //
/////////////////////////////////////////////////
void readData() 
{                                       
  int head = Serial.read();                         // Read the first byte
  if((millis() - last_read) > 3000)                 // If last data received over 3 seconds assume signal lost
  {
    watchdog();                                     // Call the watchdog protocol to power down the drone safely
    no_comm = true;                                 // Enable the lost signal flag
  }
  // If data is being received contiune.
  if(head == '!')                                   // if the byte is a ! (start bit of transmission) read the remaining bits 
  {   
    last_read = millis();                           // Set the last read variable to now
    for (int i = 0; i<5; i++)                       // loop 5 times to pick up the remaining data
    {
      rcvd[i] = Serial.read();                      // Read and store the values into the rcvd array
      if (rcvd[i] != 255)                           // Check if any bytes are 255 (This is an errornous reading and must be ignored) 
      {
        last[i] = rcvd[i];                          // If not errornous data store it into last array for comparison later
      }
      else                                          // If it was errornous data then resort back to last correct value received
      {
        rcvd[i] = last[i];                          // Copy the last correct value to the rcvd array
      }
    }
    Serial.print('?');                              // Send this character to signal ready for data
  } 
}

//rcvd layout:
//                 0         1      2       3      4
// [Head bit] [arm/prime] [power] [roll] [pitch] [yaw]

