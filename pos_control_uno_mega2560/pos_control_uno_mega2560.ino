
#include <SoftwareSerial.h>
#include <Metro.h>
#define RIGHT 1
#define LEFT -1
//#define AXIS_TYPE OUTWARD
#define AXIS_TYPE RIGHT
#define HOME_SPEED_PWM 150

//MOTOR PWM PIN ASSIGNMENT
#define MotorPin0 9
#define MotorPin1 10


//ENCODER PIN ASSIGNMENT
#define encoderPinA 2
#define encoderPinB 3

// STATE MACHINE
#define INIT_STATE 0
#define HOME_STATE 1
#define HOME_ZONE_STATE 2
#define WORKING_RANGE_STATE 3
#define END_ZONE_STATE 4


int state=HOME_STATE;



double cmdPos_ROS=0,anglePos_ROS=0; //degree
double pos_offset=0;
double max_working_rage=0;
double cmdPos_Joint=0,anglePos_Joint=0;
double Kp=20;
//ENC STATE
int pinAState = 0;int pinAStateOld = 0;int pinBState = 0;int pinBStateOld = 0;

volatile long encoderPos = 0;
volatile long unknownvalue = 0;

Metro mainTimer = Metro(10);

//HOME
#define HomePin 0
#define EndPin 1


void setup() { 

    pinMode(MotorPin0, OUTPUT);
    pinMode(MotorPin1, OUTPUT);

    pinMode(encoderPinA, INPUT); 
    digitalWrite(encoderPinA, HIGH); // turn on pullup resistor
    pinMode(encoderPinB, INPUT); 
    digitalWrite(encoderPinB, HIGH); // turn on pullup resistor

    attachInterrupt(0, doEncoder, CHANGE); // encoder pin on interrupt 0 - pin 2
    attachInterrupt(1, doEncoder, CHANGE);

    Serial.begin (57600);
} 

void loop(){
  if (mainTimer.check() == true) { 
  
    //READ HOME SENSOR VALUE
    int HomeValue=analogRead(HomePin);
    int EndValue=analogRead(EndPin);

    //GET CMD FROM MEGA
    get_cmd_pos();
    
    //CREATE cmdPos_Joint
    cmdPos_Joint=cmdPos_ROS+pos_offset;
    //GET ENC
    get_angle_from_enc();

    
    //CONTROL STRATERGY
    switch(state){

      case INIT_STATE:
            go_joint_pos_ros(cmdPos_ROS);
	          break;

      
      case HOME_STATE:
          
            if(HomeValue>650) {
              	pos_offset=anglePos_Joint;
                state=HOME_ZONE_STATE;

            }
            else{
              if (AXIS_TYPE==RIGHT){
                  send_cmd_to_motor(-HOME_SPEED_PWM);
              }
              else {
                  send_cmd_to_motor(HOME_SPEED_PWM);
              }
            	    
            }
	        
            break;

      case HOME_ZONE_STATE:
      	    if(cmdPos_ROS>anglePos_ROS) {
                  go_joint_pos_ros(cmdPos_ROS);
                  state=WORKING_RANGE_STATE;
      	    }
           else
                  go_joint_pos_ros(0);
           break;

      case WORKING_RANGE_STATE:
            if(HomeValue>650) {
                state=HOME_ZONE_STATE;
            }
            else if (EndValue>650){
                max_working_rage=anglePos_ROS;
                state=END_ZONE_STATE;
              } 
              else
              {
                  go_joint_pos_ros(cmdPos_ROS);            
              }
            break;
      case END_ZONE_STATE:
            if(cmdPos_ROS<anglePos_ROS) {
                  go_joint_pos_ros(cmdPos_ROS);
                  state=WORKING_RANGE_STATE;
            }
           else {
                  go_joint_pos_ros(max_working_rage);  
           }
           break;
   
      }


    


    /////WRITE ENC to mega
    
    int x=cmdPos_ROS;  
    char xS='{';
    byte xH =highByte(x);
    byte xL =lowByte(x);
    char xF='}'; 
    Serial.print(xS);Serial.write(xH);Serial.write(xL);Serial.write(xF);
    
    
    }
}


void go_joint_pos_ros(double target_pos_ros) {
      double target_pos_joint=target_pos_ros+pos_offset;
      double cmdPwm=Kp*(target_pos_joint-anglePos_Joint);
      send_cmd_to_motor(cmdPwm);
        
      if (AXIS_TYPE==RIGHT){
          send_cmd_to_motor(cmdPwm);
      }
      else {
          send_cmd_to_motor(-cmdPwm);
      }
     }


void get_cmd_pos() {

    //GET POS_CMD FROM MEGA

     char rS=(char)Serial.read();
     if(rS=='{'){
     char commandArray[3];
     Serial.readBytes(commandArray,3);
     byte rH=commandArray[0];
     byte rL=commandArray[1];
     char rF=commandArray[2];
     if(rF=='}')         
       cmdPos_ROS=(double)((rH<<8)+rL); 

     }

}


void get_angle_from_enc() {

    if (AXIS_TYPE==RIGHT){
          anglePos_Joint=-1*encoderPos*0.00694;//(double)360/(64*810*1);
          anglePos_ROS=anglePos_Joint-pos_offset;
    }
    else {
          anglePos_Joint=1*encoderPos*0.00694;//(double)360/(64*810*1);      
          anglePos_ROS=anglePos_Joint-pos_offset;
    }
}



void send_cmd_to_motor(int cmdpwm) {

    int vPlus=0,vMinus=0;
    if(cmdpwm>=0)
    {
      vPlus=cmdpwm;
      vMinus=0;
    }
    else if(cmdpwm<0)
     {
      vPlus=0;
      vMinus=-cmdpwm;
    }   
    
     if(vPlus>=255)
        vPlus=255;
     else if(vPlus<-255)
          vPlus=-255;
          
          
     if(vMinus>=255)
        vMinus=255;
     else if(vMinus<-255)
          vMinus=-255;      
    analogWrite(MotorPin1,vPlus);
    analogWrite(MotorPin0,vMinus);
 
}







void doEncoder() {
    // encoderPos++;
    pinAState = digitalRead(2);
    pinBState = digitalRead(3);

    if (pinAState == 0 && pinBState == 0) {
    if (pinAStateOld == 1 && pinBStateOld == 0) // forward
    encoderPos ++;
    if (pinAStateOld == 0 && pinBStateOld == 1) // reverse
    encoderPos --;
    if (pinAStateOld == 1 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    if (pinAStateOld == 0 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    }
    if (pinAState == 0 && pinBState == 1) {
    if (pinAStateOld == 0 && pinBStateOld == 0) // forward
    encoderPos ++;
    if (pinAStateOld == 1 && pinBStateOld == 1) // reverse
    encoderPos --;
    if (pinAStateOld == 1 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    if (pinAStateOld == 0 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    }
    if (pinAState == 1 && pinBState == 1) {
    if (pinAStateOld == 0 && pinBStateOld == 1) // forward
    encoderPos ++;
    if (pinAStateOld == 1 && pinBStateOld == 0) // reverse
    encoderPos --;
    if (pinAStateOld == 0 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    if (pinAStateOld == 1 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    }

    if (pinAState == 1 && pinBState == 0) {
    if (pinAStateOld == 1 && pinBStateOld == 1) // forward
    encoderPos ++;
    if (pinAStateOld == 0 && pinBStateOld == 0) // reverse
    encoderPos --;
    if (pinAStateOld == 0 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    if (pinAStateOld == 1 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    }
    pinAStateOld = pinAState;
    pinBStateOld = pinBState;
}


/*
Serial.print("Encoder: "); Serial.print(encoderPos); 
Serial.print(" unKnown: ");Serial.print(unknownvalue, DEC); 
Serial.println(""); 
*/
