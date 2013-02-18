/*
  motor.h - Library for motor control
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef MOTOR_h
#define MOTOR_h

#include <wiring.h> // used for analogic read function (core lib)
#include <PID.h>


#define SPEED_ERROR -1
#define OBSTACLE -2
#define TIMEOUT -3
#define BAD_ANGLE -4
#define COMPASS_ERROR -5
#define CAMERA_ERROR -6
#define XBEE_ERROR -7
#define FILE_OPEN_ERROR -1000
#define FILE_CLOSE_ERROR -1001


#define CMD_START         0x01
#define CMD_STOP          0x02
#define CMD_INFOS         0x03
#define CMD_PICTURE       0x04
#define CMD_TURN_RIGHT    0x05
#define CMD_TURN_LEFT     0x06
#define CMD_CHECK_AROUND  0x07
#define CMD_MOVE_TILT_PAN 0x08

#define RESP_SIZE 8
#define RESP_INFOS        0x01
#define RESP_CHECK_AROUND 0x02

#define STATE_STOP 0x00
#define STATE_GO   0x01

#define SPEEDNOMINAL 150
#define SPEEDMAX 255     //(255=PWM max)
#define SPEEDDELTA 50     

#define LEFT_DIRECTION 1
#define RIGHT_DIRECTION 2

#define BOTH_MOTOR 0
#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2

#define DISTANCE_MIN 50 // 50 cm before stopping, must be > 20cm which is lower range of IR sensor

#define In1MotorRight1Pin  26     // In1 pin of L293D #1 for motor right #1 connected to digital pin J6-02(PMD0/RE0)
#define In2MotorRight1Pin  27     // In2 pin of L293D #1 for motor right #1 connected to digital pin J6-04(PMD1/RE1)
#define EnableMotorRight1Pin 3    // Enable pin of L293D #1 for motor right #1 connected to PWM pin J6-07(OC1/RD0)   Use TIMER_OC1
    
#define In1MotorRight2Pin  28     // In1 pin of L293D #2 for motor right #2 connected to digital pin J6-06(PMD2/RE2)
#define In2MotorRight2Pin  29     // In2 pin of L293D #2 for motor right #2 connected to digital pin J6-08(PMD3/RE3)
#define EnableMotorRight2Pin 5    // Enable pin of L293D #2 for motor right #2 connected to PWM pin J6-11(OC2/RD1)   Use TIMER_OC2
    
#define In1MotorLeft1Pin  30      // In1 pin of L293D #1 for motor left #1 connected to digital pin J6-10(PMD4/RE4)
#define In2MotorLeft1Pin  31      // In2 pin of L293D #1 for motor left #1 connected to digital pin J6-12(PMD5/RE5)
#define EnableMotorLeft1Pin 6     // Enable pin of L293D #1 for motor left #1 connected to PWM pin J6-13(OC3/RD2)    Use TIMER_OC3
    
#define In1MotorLeft2Pin  32      // In1 pin of L293D #2 for motor left #2 connected to digital pin J6-14(PMD6/RE6)
#define In2MotorLeft2Pin  33      // In2 pin of L293D #2 for motor left #2 connected to digital pin J6-16(PMD7/RE7)
#define EnableMotorLeft2Pin 9     // Enable pin of L293D #2 for motor left #2 connected to PWM pin J5-03(OC4/RD3)    Use TIMER_OC4
    
#define EncodeurTickRightINT  1  // INT used by the encodeur for motor right connected to interrupt pin INT1 J6-05(INT1) Use INT1
#define EncodeurTickLeftINT   2  // INT used by the encodeur for motor left  connected to interrupt pin INT2 J6-15(INT2)  Use INT2


#define GP2Y0A21YK_Pin 14   // IR sensor GP2Y0A21YK analogic pin J7-01 A0 (C2IN-/AN2/SS1/CN4/RB2)   Use ADC module channel 2
/* Analogic interface is provided on pin V0 */
/* Power +3V is set on pin VCC              */
/* Ground    is set on pin GND              */

#define IRSERVO_Pin 34   // IR Servo pin connected to digital pin J5-02 (PMRD/CN14/RD5)
/* Power +5V */
/* Ground    */
#define HSERVO_Pin  36   // Horizontal Servo pin connected to digital pin J5-06 (CN15/RD6)
/* Power +5V */
/* Ground    */
#define VSERVO_Pin  37   // Vertical Servo pin connected to digital pin J5-08 (CN16/RD7)
/* Power +5V */
/* Ground    */

/* Compas                                                           */
/* I2C interface is provided on pins:                               */
/*         1 = Power +5V                                            */                     
/*         2 = SCL connected to A5/J7-11(SCL1/RG2) selected by JP8  */
/*         3 = SDA connected to A4/J7-09(SDA1/RG3) selected by JP6  */
/*         9 = Ground                                               */


#define SS_CS_Pin  4   // Serial Select pin connected to digital pin J6-09 (RF1)
/* SD-Card                                                                                          */
/* SPI interface is provided on pins:                                                               */                    
/*         SPI/SD-Card Master Output (MOSI) connected to J5-07(SDO2/PMA3/CN10/RG8) selected by JP5  */
/*         SPI/SD-Card Master Input  (MISO) connected to J5-09(SDI2/PMA5/CN8/RG7) selected by JP7   */
/*         SPI/SD-Card Serial Clock  (SCLK) connected to J5-11 (SCK2/PMA5/CN8/RG6)                  */                                            


int motor_init();     // initialize everything
int mainRobot ();


void IntrTickRight();  // interrupt handler encodeur right
void IntrTickLeft();   // interrupt handler encodeur right



void forward();     
/* Description: set IN1 and IN2 of the 4 motors in order to run clockwise     */                                            
/*              (refer truth table LM293D)                                    */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */

void forward_test(int num); 
/* Description: set IN1 and IN2 of the motor num in order to run clockwise    */                                            
/*              (refer truth table LM293D)                                    */
/*              used for testing                                              */
/* input:       num                                                           */ 
/*                  = 1: In1MotorRight1Pin                                    */ 
/*                  = 2: In1MotorRight2Pin                                    */ 
/*                  = 3: In1MotorLeft1Pin                                     */ 
/*                  = 4: In1MotorLeft2Pin                                     */                       
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */

void start_forward();
/* Description: call forward +                                                */ 
/*              set enable pin of the 4 motors to SPEEDNOMINAL                */
/* input:       none                                                          */
/* output:      none                                                          */                       
/* lib:         forward                                                       */
/*              analogWrite                                                   */
                                   
void start_forward_test(int num);
/* Description: call forward +                                                */ 
/*              set enable pin of the 4 motors to SPEEDNOMINAL                */                                           
/*              (refer truth table LM293D)                                    */
/*              used for testing                                              */
/* input:       num                                                           */ 
/*                  = 1: In1MotorRight1Pin                                    */ 
/*                  = 2: In1MotorRight2Pin                                    */ 
/*                  = 3: In1MotorLeft1Pin                                     */ 
/*                  = 4: In1MotorLeft2Pin                                     */ 
/* output:      none                                                          */
/* lib:         forward                                                       */
/*              analogWrite                                                   */

void backward();
/* Description: set IN1 and IN2 of the 4 motors in order to run anti-clockwise*/                                            
/*              (refer truth table LM293D)                                    */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         digitalWrite                                                  */

void start_backward();
/* Description: call backward +                                               */ 
/*              set enable pin of the 4 motors to SPEEDNOMINAL                */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         backward                                                      */
/*              analogWrite                                                   */

void stop();
/* Description: set IN1 and IN2 of the 4 motors in order to stop              */
/*              (refer truth table LM293D)                                    */
/*              and reset enable pin of the 4 motors                          */
/* input:       none                                                          */
/* output:      none                                                          */
/* lib:         analogWrite                                                   */
/*              digitalWrite                                                  */


int accelerate (int motor);
/* Description: set enable pin of the corresponding motors to an higher value */
/*              (one increment)                                               */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed computed > SPEEDMAX                */ 
/*                  = SUCCESS otherwise                                       */ 
/* lib:         analogWrite                                                   */

int accelerate_n (int motor, int n);
/* Description: set enable pin of the corresponding motors to an higher value */
/*              (n increments)                                                */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* input:       n                                                             */
/*                  = number of increments                                    */  
/* output:      return                                                        */                            
/*                  = return number of increments done                        */ 
/* lib:         accelerate                                                    */


int deccelerate(int motor);
/* Description: set enable pin of the corresponding motors to a lower value   */
/*              (one decrement)                                               */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed = 0                                */ 
/*                  = SUCCESS otherwise                                       */                             
/* lib:         analogWrite                                                   */
                                                                   
int deccelerate_n(int motor, int n);
/* Description: set enable pin of the corresponding motors to a lower value   */
/*              (n decrements)                                                */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/*                  = BOTH_MOTOR                                              */ 
/* input:       n                                                             */
/*                  = number of decrements                                    */  
/* output:      return                                                        */                            
/*                  = return number of decrements done                        */                                     
/* lib:         deccelerate                                                   */

 
 
int adjustMotor (int motor, int pid);
/* Description: Adjust the speed of the motor according the PID value         */
/* input:       motor                                                         */ 
/*                  = LEFT_MOTOR                                              */ 
/*                  = RIGHT_MOTOR                                             */ 
/* input:       pid                                                           */
/*                  = pid value to adjust                                     */  
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed computed > SPEEDMAX                */ 
/*                  = SUCCESS otherwise                                       */                                        
/* lib:         analogWrite                                                   */                                        
                                                                      
                                     
int go(int d, int pid_ind); 
/* Description: go during d ticks using PID adjustement if pid_ind = 1,       */
/*              control motors speed between left and right                   */
/*              using a tick counter provided by an encoder                   */
/*              adjust (calling adjustMotor) the motor speed if necessary     */
/*              using a PID method                                            */
/* input:       d                                                             */ 
/*                  = number of right+left ticks to go                        */ 
/* input:       pid_ind                                                       */
/*                  = 0: disable PID adjustement                              */
/*                  = 1: enable PID adjustement                               */    
/* output:      return                                                        */                            
/*                  = SPEED_ERROR if speed computed > SPEEDMAX                */
/*                  = OBSTACLE if an obstacle is detected before DISTANCE_MIN */
/*                  = SUCCESS otherwise                                       */                                          
/* lib:         computePID                                                    */                                
/*              adjustMotor                                                   */                                
/*              GP2Y0A21YK_getDistanceCentimeter                              */                                      
                                       
                                       
int check_around();
/* Description: move the servo used by the IR sensor in order to determine    */
/*              the direction without obstacle                                */
/* input:       none                                                          */   
/* output:      return                                                        */                            
/*                  = OBSTACLE if an obstacle is detected for both directions */
/*                  = LEFT_DIRECTION if best direction to go                  */ 
/*                  = RIGHT_DIRECTION if best direction to go                 */                      
/* lib:         IRServo.attach                                                */                                
/*              IRServo.write                                                 */                                
/*              delay                                                         */
/*              adjustMotor                                                   */                                
/*              GP2Y0A21YK_getDistanceCentimeter                              */  


                      
int turn(double alpha, unsigned long timeout);
/* Description: turns with an angle of alpha degrees before a delay (timeout) */
/*              using a compass to get the direction                          */
/* input:       alpha                                                         */ 
/*                  = angle to turn (-180 < alplha < +180) and alpha <> 0     */
/*              timeout                                                       */ 
/*                  = timeout in ms                                           */      
/* output:      return                                                        */                            
/*                  = BAD_ANGLE if not (-180 < alplha < +180) and alpha <> 0  */
/*                  = COMPASS_ERROR if an error occurs with the compass       */
/*                  = TIMEOUT if turn is not completed before the delay       */
/*                  = SUCCESS otherwise                                       */  
/* lib:         CMPS03_read()                                                 */                                
/*              accelerate_n                                                  */                                
/*              deccelerate_n                                                 */
/*              millis                                                        */                                


void move_Tilt_Pan(uint8_t HPos, uint8_t VPos);
/* Description: move the Tilt&Pan depending on the                            */
/*              Horizontal and Vertical position                              */
/* input:       HPos                                                          */ 
/*                  = Horizontal position to move the Tilt&Pan (0< HPos <180) */
/*              VPos                                                          */ 
/*                  = Vertical position to move the Tilt&Pan  (0< VPos <180)  */      
/* output:      none                                                          */                            
/* lib:         IRServo.attach                                                */                                
/*              IRServo.write                                                 */                                
/*              delay                                                         */


int makePicture (int n);
/* Description: make a picture using the JPEGCamera lib                       */
/*              and save it on a SD card in a file PICTxx.jpg                 */
/* input:       n                                                             */ 
/*                  = picture number xx                                       */     
/* output:      return                                                        */                            
/*                  = FILE_OPEN_ERROR if an error occurs during file opening  */
/*                  = FILE_CLOSE_ERROR if an error occurs during file closing */
/*                  = CAMERA_ERROR if an error occurs with the camera         */
/*                  = SUCCESS otherwise                                       */  
/* lib:         sprintf                                                       */                                
/*              open (file)                                                   */
/*              write (file)                                                  */
/*              close (file)                                                  */                                  
/*              JPEGCamera.takePicture                                        */
/*              JPEGCamera.getSize                                            */  
/*              JPEGCamera.readData                                           */
/*              JPEGCamera.stopPictures                                       */

int sendPicture (int n);
/* Description: send using XBee interface a picture stored                    */
/*              in a file PICTxx.jpg on a SD card                             */
/* input:       n                                                             */ 
/*                  = picture number xx                                       */     
/* output:      return                                                        */                            
/*                  = FILE_OPEN_ERROR if an error occurs during file opening  */
/*                  = FILE_CLOSE_ERROR if an error occurs during file closing */
/*                  = XBEE_ERROR if an error occurs with the XBee interface   */
/*                  = SUCCESS otherwise                                       */ 
/* lib:         sprintf                                                       */                                
/*              open (file)                                                   */
/*              read (file)                                                   */
/*              close (file)                                                  */                                  
/*              xBTsendXbee                                                   */

#endif