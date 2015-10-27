
#include <SoftwareSerial.h>
#include <Metro.h>


//MOTOR PWM PIN ASSIGNMENT
#define MotorPin0 9
#define MotorPin1 10


//ENCODER PIN ASSIGNMENT
#define encoderPinA 2
#define encoderPinB 3

// STATE MACHINE
#define HOME_STATE 0
#define CONTROL_STATE 1
int state=HOME_STATE;
int cmdPos=0; //degree

//ENC STATE
int pinAState = 0;int pinAStateOld = 0;int pinBState = 0;int pinBStateOld = 0;

volatile long encoderPos = 0;
volatile long unknownvalue = 0;

Metro mainTimer = Metro(10);

//HOME
#define HomePin 0



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


    //GET CMD FROM MEGA
    get_cmd_pos();

    //GET ENC
    double anglePos=get_angle_from_enc();
    double cmdPwm=0;
    //CONTROL STRATERGY
    switch(state){
      
      case HOME_STATE:
            cmdPwm=10*(cmdPos-anglePos);
            if(HomeValue>600) {
              send_cmd_to_motor(0);
            }
            else{
            //SEND CMD
            send_cmd_to_motor(cmdPwm);
            }
            break;
      case CONTROL_STATE:
            cmdPwm=10*(cmdPos-anglePos);
            //SEND CMD
            send_cmd_to_motor(cmdPwm);
            break;
      
      }


    


    /////WRITE ENC to mega
    
    int x=HomeValue;  
    char xS='{';
    byte xH =highByte(x);
    byte xL =lowByte(x);
    char xF='}'; 
    Serial.print(xS);Serial.write(xH);Serial.write(xL);Serial.write(xF);
    
    
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
       cmdPos=(rH<<8)+rL; 
     // ACTUALLY NOW WE DONT NEED TO SEND ENC TO MEGA        
     //Serial.print("  cmd  ");Serial.print(cmd0Pos);
     //Serial.print("  rS  ");Serial.print(rS);
     //Serial.print("  rH  ");Serial.print(rH);
     //Serial.print("  rL  ");Serial.print(rL);
     //Serial.print("  rF  ");Serial.println(rF);

     }

}


double get_angle_from_enc() {

    double result=-1*encoderPos*0.00694;//(double)360/(64*810*1);
    return result;
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
