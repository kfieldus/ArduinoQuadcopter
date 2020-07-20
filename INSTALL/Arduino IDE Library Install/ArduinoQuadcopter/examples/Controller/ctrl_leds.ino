// This method is used to toggle the armed LED's to indicate the current armed status locally on the controller
void set_armed(int a)
{
   if (a == HIGH)                                 // If the armed status is true
   {
    digitalWrite(aled, HIGH);                     // Turn on the Armed LED signal
   } 

  if (a == LOW)                                   // If the armed status is false 
  {
    primed = LOW;                                 // Ensure that primed is also false
    digitalWrite(aled, LOW);                      // Disable the armed LED signal
    digitalWrite(pled, LOW);                      // Disable the primed LED signal;

  }
}

// This method is used to toggle the primed LED's to indicate the current primed status locally on the controller
void set_primed(int b)
{
   if (b == HIGH)                                 //  If the primed status is true 
   {
     digitalWrite(pled, HIGH);                    // Turn on the Primed LED signal
   } 

   if (b == LOW)                                  // If the primed status is false
   {
    digitalWrite(pled, LOW);                      // Turn off the Primed LED signal
   }
}

