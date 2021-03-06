#include "WProgram.h"

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"
#include "POutput.h"
#include "PRX.h"
#include "PSensors.h"



uint32_t currentTime = 0;
uint16_t calibratingA = 0;  
uint16_t calibratingG = 0;

static int16_t last_error[3] = {0,0,0};
static int16_t sum_error[3] = {0,0,0};
double Kp[3] = {0.6,0.6,0.6};
double Ki[3] = {0.1,0.1,0.1};
double Kd[3] = {0.3,0.3,0.3};
int16_t gyroZero[3] = {0,0,0};
int16_t accZero[3] = {0,0,0};
imu_t imu;

int16_t rcData[RC_CHANS];    // interval [1000;2000]
int16_t rcSerial[8];         // interval [1000;2000] - is rcData coming from MSP
int16_t rcCommand[4];        // interval [1000;2000] for THROTTLE and [-500;+500] for ROLL/PITCH/YAW
uint8_t rcSerialCount = 0;   // a counter to select legacy RX when there is no more MSP rc serial data


double axisPID[3];
double c_angle[2];



bool MultiWii_setup() {
    
  Serial.begin(9600); // initialize serial 
  
#if defined(TRACE)
  Serial.println(">Start Init");
#endif 
  
  initOutput();
       
  configureReceiver();

  if (!initSensors())
  {   
#if defined(TRACE)	  
   Serial.println("<End KO initSensors");
#endif    
    return false;
  }

#if defined(TRACE)
  Serial.println("<End OK Init");
#endif 
  
/* START TESTCASE 1: spin up each blade individually for 10s each and check they all turn the right way  */
#if defined(TRACE)
  Serial.println("START TESTCASE 1");
#endif   

  for(int i=0; i< 4; i++)
  {
      writeOneMotor(i, MINCOMMAND);
      delay(10*1000);
  }

#if defined(TRACE)
  Serial.println("END TESTCASE 1");
#endif     
/* END TESTCASE 1 */


/* START TESTCASE 2: Spin all the motors together for 10s judging how much lift is provided  */
#if defined(TRACE)
  Serial.println("START TESTCASE 2");
#endif  

  writeAllMotors(MINCOMMAND);
  delay(10*1000); 

#if defined(TRACE)
  Serial.println("END TESTCASE 2");
#endif   
/* END TESTCASE 2 */

/* CALIBRATE */  
#if defined(TRACE)
  Serial.println("Start Calibrate");
#endif 

  calibratingG=512; 
  calibratingA=512; 

  while (calibratingG > 0 || calibratingA > 0)
  { 
     ACC_getADC();
  
     Gyro_getADC();
  }

#if defined(TRACE)
  Serial.println("End Calibrate");
  Serial.println("********************* FLY ***********************");
#endif  

  writeAllMotors(MINCOMMAND);

  return true;
}


// ******** Main Loop *********

void MultiWii_loop () {
 
  uint8_t axis;
  int16_t error= 0;
  int16_t PTerm = 0,ITerm = 0,DTerm;
  static uint32_t rcTime  = 0;
  
  
  if (currentTime > rcTime ) { // 50Hz: PPM frequency of the RC, no change happen within 20ms
    rcTime = currentTime + 20000;
    
    computeRC();
  }

  //**** Read IMU ****   
  ACC_getADC();
  
  Gyro_getADC();
  //**** PITCH & ROLL & YAW PID ****
   
  // PITCH & ROLL
  for(axis=0;axis<2;axis++) {
  	
    error =  rcCommand[axis] - (int16_t)(c_angle[axis]*159.0); // convert c_angle from -pi;+pi to -500;+500
    sum_error[axis] += error;
    axisPID[axis] =  (Kp[axis]*error) + (Ki[axis]*sum_error[axis]) + (Kd[axis]*(error - last_error[axis]));
    last_error[axis] = error;

#if defined(TRACE)  
    Serial.print(">MultiWii_loop: c_angle[");Serial.print((int)axis);Serial.print("]:");Serial.println(c_angle[axis]);
    Serial.print(">MultiWii_loop: rcCommand[");Serial.print((int)axis);Serial.print("]:");Serial.println(rcCommand[axis]);
    Serial.print(">MultiWii_loop: error:");Serial.println(error);
    Serial.print(">MultiWii_loop: axisPID[");Serial.print((int)axis);Serial.print("]:");Serial.println(axisPID[axis]);
 #endif     
  }

  //YAW
  axisPID[YAW] = 0;
  
#if defined(TRACE)  
  Serial.print(">MultiWii_loop: axisPID[");Serial.print((int)YAW);Serial.print("]:");Serial.println(axisPID[YAW]);
#endif

  RunMotors();
}
