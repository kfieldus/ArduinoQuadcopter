// Used to transmit the currebt values stored in the byte array to the Quadcopter using the Xbee Module
void output()
{
  delay(10);
  Serial.print('!');                         // Used as a start bit to look for on the receiving end
  Serial.print(ap_status);                   // Armed / Primed Status byte
  Serial.print(power);                       // Throttle 
  Serial.print(roll);                        // Roll Values  
  Serial.print(pitch);                       // Pitch Values
  Serial.print(yaw);                         // Motor 1 Trim
}

void readData()
{ 
  
  if (firstbyte == true)                    // This is used as a check that no data has been sent before, so do not wait for an ACK
  {
   output();                                // Output the control signal data.
  }
  
  int readybit = Serial.read();            // Read the serial bus to see if we have received an ACK
  if (readybit == '?')                     // If the received char is a ? the ACK has been received
  {
   output();                               // Send the data
   readybit = 0;                           // Reset the ready to send flag for next ACK to be received.

  }
}

