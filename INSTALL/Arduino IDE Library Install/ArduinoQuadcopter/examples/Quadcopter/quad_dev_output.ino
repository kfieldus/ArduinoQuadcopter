////////////////////////////////////////////////////
// This method is only used if the dev flag is set//
////////////////////////////////////////////////////
void dev_output ()
{     
      Serial.print("YAW= ");Serial.print(yawdir);Serial.print("\t");              // Print the Yaw value from the DMP
      Serial.print("PITCH= ");Serial.print(pitchdir);Serial.print("\t");          // Print the Pitch value from the DMP
      Serial.print("ROLL= ");Serial.print(rolldir);Serial.print("\t");            // Print the Roll value from the DMP
      Serial.print("YAW_G= ");Serial.print(yawgyro);Serial.print("\t");           // Print the Yaw gyro rate value from the DMP
      Serial.print("PITCH_G= ");Serial.print(pitchgyro);Serial.print("\t");       // Print the Pitch gyro rate value from the DMP
      Serial.print("ROLL_G= ");Serial.print(rollgyro);Serial.print("\t");         // Print the Roll gyro rate value from the DMP
      Serial.print("YAWPID_G= ");Serial.print(YAW_OUT);Serial.print("\t");        // Print the Yaw PID correction value.
      Serial.print("PITCHPID= ");Serial.print(PITCH_OUT);Serial.print("\t");      // Print the Pitch PID correction value. 
      Serial.print("ROLLPID= ");Serial.print(ROLL_OUT);Serial.print("\t");        // Print the Roll PID correction value.
      Serial.print("m1= ");Serial.print(m1_pid);Serial.print("\t");               // Print the motor 1 output value
      Serial.print("m2= ");Serial.print(m2_pid);Serial.print("\t");               // Print the motor 2 output value
      Serial.print("m3= ");Serial.print(m3_pid);Serial.print("\t");               // Print the motor 3 output value
      Serial.print("m4= ");Serial.println(m4_pid);                                // Print the motor 4 output value
}



