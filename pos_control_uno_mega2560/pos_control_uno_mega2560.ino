
#include <SoftwareSerial.h>
#include <Metro.h>
//#define AXIS_TYPE OUTWARD
#define CW 1 //L0 L1 L2 L3 R1
#define CCW -1 //R0
#define AXIS_TYPE CW 
#define HOME_SPEED_PWM 150 //150

//MOTOR PWM PIN ASSIGNMENT
#define MotorPin0 9
#define MotorPin1 10
//#define MotorENA 4
//#define MotorENB 5
//#define MotorPin 6
//ENCODER PIN ASSIGNMENT
#define encoderPinA 2
#define encoderPinB 3

// STATE MACHINE
#define INIT_STATE 0
#define HOME_STATE 1
#define HOME_ZONE_STATE 2
#define WORKING_RANGE_STATE 3
#define END_ZONE_STATE 4
#define MAX_PWM 250
double rad_tick=(double)6.28/32767; //resolution
double degree_tick=(double)360/32767; //resolution

int state=WORKING_RANGE_STATE;
int home_switch=0;
int offset_flag=0;


struct mySerialSendByte{
  char cS=0;
  char cS2=0;
  byte cV0H=0;
  byte cV0L=0;
  byte cV1H=0;
  byte cV1L=0;
  byte cV2H=0;
  byte cV2L=0;
  byte cV3H=0;
  byte cV3L=0;
  byte cV4H=0;
  byte cV4L=0;
  char cF=0; 
  char cF2=0;
  };
mySerialSendByte mySerialSendByte;
double cmdPwm=0;
double cmdPos_ROS=90,anglePos_ROS=0; //degree
double pos_offset=0;
int pos_offset_v=0;
double max_working_rage=0;
double cmdPos_Joint=0,anglePos_Joint=0;
double Kp=20;
//ENC STATE
int pinAState = 0;int pinAStateOld = 0;int pinBState = 0;int pinBStateOld = 0;

volatile long encoderPos = 0;
volatile long unknownvalue = 0;

Metro mainTimer = Metro(10);
Metro feedbackTimer = Metro(10);
//HOME
#define HomePin 0
#define EndPin 1


void setup() { 

    pinMode(MotorPin0, OUTPUT);
    pinMode(MotorPin1, OUTPUT);
//    pinMode(MotorENA, OUTPUT);
//    pinMode(MotorENB, OUTPUT);
//    pinMode(MotorPin, OUTPUT);  
      
    pinMode(encoderPinA, INPUT); 
    digitalWrite(encoderPinA, HIGH); // turn on pullup resistor
    pinMode(encoderPinB, INPUT); 
    digitalWrite(encoderPinB, HIGH); // turn on pullup resistor

    attachInterrupt(0, doEncoder, CHANGE); // encoder pin on interrupt 0 - pin 2
    attachInterrupt(1, doEncoder, CHANGE);

    Serial.begin (57600);

} 

void loop(){

  if (feedbackTimer.check() == true) {
      send_feedback();
      //delay(10);
  }

  
  if (mainTimer.check() == true) {
       control_loop(); 
       printTemple();
  } 
//printTemple();
}

void control_loop(){
    /////WRITE ENC to mega
    
    //READ HOME SENSOR VALUE
    int HomeValue=0;//analogRead(HomePin);
    int EndValue=0;//analogRead(EndPin);

    //GET CMD FROM MEGA
    get_cmd_pos();
    
    //CREATE cmdPos_Joint
    
    pos_offset=pos_offset+0.01*pos_offset_v;

          
    cmdPos_Joint=cmdPos_ROS+pos_offset;

    //GET ENC
    get_angle_from_enc();

    
    //CONTROL STRATERGY
    switch(state){

      case INIT_STATE:
            send_cmd_to_motor(0);
            if(home_switch==1)
              state=HOME_STATE;
            break;
       
      case HOME_ZONE_STATE:
            if(home_switch==1)
              go_joint_pos_ros(0);
            else{
              if(cmdPos_ROS>anglePos_ROS) {
                    go_joint_pos_ros(cmdPos_ROS);
                    state=WORKING_RANGE_STATE;
              }
              else
                    go_joint_pos_ros(0);
            }
            break;


            
      case WORKING_RANGE_STATE:
            if(home_switch==1)
              state=HOME_STATE;
            else {
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
            }

           
            break;
 
      } 
  }


void send_feedback(){

    int V[3]={state,(int)anglePos_ROS,(int)cmdPos_ROS};
    //int V[3]={1,2,3}; 
    mySerialSendByte.cS='{';  
    mySerialSendByte.cV0H =highByte(V[0]);
    mySerialSendByte.cV0L =lowByte(V[0]);
    
    mySerialSendByte.cV1H =highByte(V[1]);
    mySerialSendByte.cV1L =lowByte(V[1]);

    mySerialSendByte.cV2H =highByte(V[2]);
    mySerialSendByte.cV2L =lowByte(V[2]);
        
    mySerialSendByte.cF='.'; 
    mySerialSendByte.cF2='}';  

    byte sendBuffer[10] = {0};//[12];
    sendBuffer[0]=mySerialSendByte.cS;
    sendBuffer[1]=mySerialSendByte.cV0H;
    sendBuffer[2]=mySerialSendByte.cV0L;
    sendBuffer[3]=mySerialSendByte.cV1H;
    sendBuffer[4]=mySerialSendByte.cV1L;
    sendBuffer[5]=mySerialSendByte.cV2H;
    sendBuffer[6]=mySerialSendByte.cV2L;
    sendBuffer[7]=highByte(V[0]+V[1]+V[2]);
    sendBuffer[8]=lowByte(V[0]+V[1]+V[2]);   
    sendBuffer[9]=mySerialSendByte.cF2;
    //Serial.write(sendBuffer,1);  
    Serial.write(sendBuffer,sizeof(sendBuffer));


    
  }

void go_joint_pos_ros(double target_pos_ros) {
      double target_pos_joint=target_pos_ros+pos_offset;
      cmdPwm=Kp*(target_pos_joint-anglePos_Joint);
  if(AXIS_TYPE == CCW)
      send_cmd_to_motor(-cmdPwm);
      //send_cmd_to_motor_VNH2SP30(cmdPwm,-1);
  else
      send_cmd_to_motor(cmdPwm);
      //send_cmd_to_motor_VNH2SP30(250,-1);
     }


void get_cmd_pos() {

    //GET POS_CMD FROM MEGA

     char rS=(char)Serial.read();
     if(rS=='{'){
     char commandArray[5];
     Serial.readBytes(commandArray,5);
     byte rSH=commandArray[0];
     byte rSL=commandArray[1];
     byte rPH=commandArray[2];
     byte rPL=commandArray[3];
     char rF=commandArray[4];
     if(rF=='}') { 
       pos_offset_v=((rSH<<8)+rSL)*0.01;        
       cmdPos_ROS=((rPH<<8)+rPL); //degree
       //cmdPos_ROS=quantity*rad_tick*(double)360/6.28;
       }
     }

}


void get_angle_from_enc() {
  
  if(AXIS_TYPE == CCW){
          anglePos_Joint=1*encoderPos*0.00694;//(double)360/(64*810*1);      
          anglePos_ROS=anglePos_Joint-pos_offset;
  }
  else{
          anglePos_Joint=-1*encoderPos*0.00694;//(double)360/(64*810*1);      
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
    
     if(vPlus>=MAX_PWM)
        vPlus=MAX_PWM;
     else if(vPlus<-MAX_PWM)
          vPlus=-MAX_PWM;
          
          
     if(vMinus>=MAX_PWM)
        vMinus=MAX_PWM;
     else if(vMinus<-MAX_PWM)
          vMinus=-MAX_PWM;      
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



void printTemple(){
        Serial.print(" cmdPos_ROS: "); Serial.print(cmdPos_ROS); 
        Serial.print(" cmdPos_Joint: ");Serial.print(cmdPos_ROS); 
        Serial.print(" anglePos_ROS: "); Serial.print(anglePos_ROS); 
        Serial.print(" anglePos_Joint: ");Serial.print(anglePos_Joint); 
        Serial.print(" cmdPwm: ");Serial.print(cmdPwm); 
        Serial.println("");     
  
  }

