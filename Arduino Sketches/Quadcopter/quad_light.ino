/////////////////////////////////////////////////
// Method to change the colour of the RGB LED  //
/////////////////////////////////////////////////
void led_color(int r, int g, int b) {
  #ifndef DISABLE_LED                        // Only do the following if the LED flag is enabled
  analogWrite(red, r);                       // Send the value of r to the Red pin
  analogWrite(green, g);                     // Send the value of g to the Green pin 
  analogWrite(blue, b);                      // Send the value of b to the Blue pin
  #endif
}


