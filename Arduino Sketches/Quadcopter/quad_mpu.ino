
// The core functionality for reading from the DMP sensor is implemented from the I2CDev library and Motion Apps
// Library developed by Jeff Rowberg at I2Cdevlib. Within this sketch file I extended the ability to read from
// The MPU6050 DMP sensor and added a bias calcuation method to give the quadcopter a true 0 level at startup.


void initialiseMPU() {
  #ifdef DEV_MODE
  Serial.println("Initialising MPU...");
  #endif
  Wire.begin();                              // Start the i2c bus
  mpu.initialize();                          // Start the initialising sequence for the MPU6050
  devStatus = mpu.dmpInitialize();           // Set the status to the code returned from initialize()
  mpu.setXGyroOffset(220);                   // Correction value for Raw Gyro X values 220
  mpu.setYGyroOffset(75);                    // Correction value for Raw Gyro Y values 76
  mpu.setZGyroOffset(-86);                   // Correction value for Raw Gyro Z values
  mpu.setZAccelOffset(1688);                 // 1688 factory default for my test chip //1788
  mpu.setDMPEnabled(true);                   // Enable the MPU6050 
  attachInterrupt(0, dmpDataReady, RISING);  // Enabling the interrupt routine on the i2c bus
  mpuIntStatus = mpu.getIntStatus();         // Check the status returned from the MPU6050
  dmpReady = true;                           // Set ready to true
  packetSize = mpu.dmpGetFIFOPacketSize();   // Check to see if the FIFO buffer has data to be read
  #ifdef DEV_MODE
  Serial.println("Initialising MPU...Done");
  #endif
}

/////////////////////////////////////////////////
//  Method to read values from MPU6050 chip    //
/////////////////////////////////////////////////
void readMPU() {
  if (mpu_bias_calculated == false)                                      // If we haven't yet calcuated a starting bias
  {
    mpu_bias();                                                          // Calculate the bias now
  } 
  mpuInterrupt = false;                                                  // Clear the intterupt flag for the MPU6050
  mpuIntStatus = mpu.getIntStatus();                                     // Get the status of the MPU
  fifoCount = mpu.getFIFOCount();                                        // get current FIFO count
  
  if ((mpuIntStatus & 0x10) || fifoCount >=1024) 
  {
   mpu.resetFIFO(); 
  }
  if (mpuIntStatus & 0x02) 
  {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();       // Wait for correct available data length, should be a VERY short wait
    mpu.getFIFOBytes(fifoBuffer, packetSize);                            // Read a packet from FIFO
    fifoCount -= packetSize;                                             // Track FIFO count here in case there is > 1 packet available
    mpu.dmpGetQuaternion(&q, fifoBuffer);                                // Fetch the readings in Quaternions
    mpu.dmpGetGravity(&gravity, &q);                                     // Fetch the readings for Gravity    
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);                           // Get the Yaw Pitch and Roll Angles
    yawdir = (int)(ypr[0] * 180/M_PI) -yb; //- ctrl_yaw;                 // Set Yaw to the yaw readings obtained minus the calculated bias
    pitchdir = (int)(ypr[1] * 180/M_PI) -pb; //- ctrl_pitch;             // Set Yaw to the pitch readings obtained minus the calculated bias
    rolldir = (int)(ypr[2] * 180/M_PI) -rb; //- ctrl_roll;               // Set Yaw to the roll readings obtained minus the calculated bia
   // Found the moethod .getRotationZ/X/Y in the MPU6050 library.
    yawgyro = (int)((mpu.getRotationZ()/10) - yrb);
    pitchgyro = (int)((mpu.getRotationY()/10) - prb);
    rollgyro = (int)((mpu.getRotationX()/10) - rrb);

}

 
 }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Method to calculate the bias of axis on. This method extends the method for reading the MPU6050 DMP sensor and calculates and average to be used 
// as a bias.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mpu_bias()
{
  #ifdef DEV_MODE
  Serial.println("Calculating MPU Bias...");
  #endif
  led_color(200,200,0);                                                  // Change the LED to orange / Yellow
  while(millis() - started < 20000UL && mpu_bias_calculated ==   false)  // So the following for 20 seconds 
  {                                                                      // They large wait is to allow the DMP to stablise
    mpuInterrupt = false;                                                // Clear the intterupt flag for the MPU6050
    mpuIntStatus = mpu.getIntStatus();                                   // Get the status of the MPU
    fifoCount = mpu.getFIFOCount();                                      // Get current FIFO count
    if (mpuIntStatus & 0x02) {                                           // If mpu status matches 0x02
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();     // Wait for correct available data length
      mpu.getFIFOBytes(fifoBuffer, packetSize);                          // Read a packet from FIFO
      fifoCount -= packetSize;                                           // Track FIFO count here in case there is > 1 packet available
      mpu.dmpGetQuaternion(&q, fifoBuffer);                              // Fetch the readings in Quaternio
      mpu.dmpGetGravity(&gravity, &q);                                   // Fetch the readings for Gravity
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);                         // Get the Yaw Pitch and Roll Angles
      yb = (int)(ypr[0] * 180/M_PI);                                          // Set the yaw bias to yaw reading 
      pb = (int)(ypr[1] * 180/M_PI);                                          // Set the pitch bias to pitch reading
      rb = (int)(ypr[2] * 180/M_PI);                                          // Set the roll bias to roll reading
     
     yrb = (mpu.getRotationZ() / 10);          // dividing the readsings by 10 drops the gyro rotation valuefrom the 100's to the 10's 
     prb = (mpu.getRotationY() / 10);          // dividing the readsings by 10 drops the gyro rotation valuefrom the 100's to the 10's
     rrb = (mpu.getRotationX() / 10);          // dividing the readsings by 10 drops the gyro rotation valuefrom the 100's to the 10's
 
 
  }
  }
  mpu_bias_calculated = true;                                            // Set the bias calcuated flag to true
  led_color(255,0,255);                                                  // Change the LED to purple when done
  #ifdef DEV_MODE
  Serial.println("Calculating MPU Bias...Done");                         // Output a dev mode text output for the completion of this processs.
  Serial.println("Ready to fly......");
  #endif
}

/////////////////////////////////////////////////
//     Method to set DMP to ready state        //
/////////////////////////////////////////////////
void dmpDataReady() {
  mpuInterrupt = true;                                                   // Enable the interrupt routine for the MPU6050 
}


