void initialisePID() {
  #ifdef DEV_MODE                                                    // If dev mode is on, print the following message
  Serial.println("Initialising PIDs...");                            // Print status of PID to the Serial monitor
  #endif  
  PIDroll.ChangeParameters(ROLL_P,ROLL_I,ROLL_D,-200,200);          // Set the kp, ki, kd and minimum and maximum output limits for the PID control loop
  PIDpitch.ChangeParameters(PITCH_P,PITCH_I,PITCH_D,-200,200);      // Set the kp, ki, kd and minimum and maximum output limits for the PID control loop
  PIDyaw.ChangeParameters(YAW_P,YAW_I,YAW_D,-200,200);              // Set the kp, ki, kd and minimum and maximum output limits for the PID control loop
  #ifdef DEV_MODE
  Serial.println("Initialising PIDs...Done");
  #endif
}


/////////////////////////////////////////////////
//  Method to calculate PID correction values  //
/////////////////////////////////////////////////
void updatePID()
{ 
  PITCH_OUT = (int)PIDpitch.Compute((float)pitchdir - pitchgyro - ctrl_pitch);  //Calculate the PID output corretion value, taking the gyro rate, anglular rate and controller input as inputs
  ROLL_OUT = (int)PIDroll.Compute((float)rolldir - rollgyro - ctrl_roll);       //Calculate the PID output corretion value, taking the gyro rate, anglular rate and controller input as inputs
  YAW_OUT = (int)PIDyaw.Compute((float)yawdir - yawgyro - ctrl_yaw);            //Calculate the PID output corretion value, taking the gyro rate, anglular rate and controller input as inputs
    
   if ( !primed || !armed)                                                     // If not armed or primed
   {
      PIDroll.resetI();                                                        // Reset the integral step of PID to prevent windup
      PIDpitch.resetI();                                                       // Reset the integral step of PID to prevent windup
      PIDyaw.resetI();                                                         // Reset the integral step of PID to prevent windup
   } 

  
 if (primed && armed) {
    m1_pid = (int)(MOTOR_IDLE + power - PITCH_OUT + ROLL_OUT - YAW_OUT);      // Combine the PID ouput values with the pilot's throttle instruction
    m2_pid = (int)(MOTOR_IDLE + power - PITCH_OUT - ROLL_OUT + YAW_OUT);      // Combine the PID ouput values with the pilot's throttle instruction
    m3_pid = (int)(MOTOR_IDLE + power + PITCH_OUT + ROLL_OUT + YAW_OUT);      // Combine the PID ouput values with the pilot's throttle instruction
    m4_pid = (int)(MOTOR_IDLE + power + PITCH_OUT - ROLL_OUT - YAW_OUT);      // Combine the PID ouput values with the pilot's throttle instruction
    
    change_speed(m1_pid, m2_pid, m3_pid, m4_pid);                              // Send the calcuated values to the motors now.
 }

}





