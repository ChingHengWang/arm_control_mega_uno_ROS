
#include <SoftwareSerial.h>
#include <Metro.h>


//MOTOR PWM PIN ASSIGNMENT
#define MotorPin0 9
#define MotorPin1 10


//ENCODER PIN ASSIGNMENT
#define encoder0PinA 2
#define encoder0PinB 3

int x=0;

//INTEGER CMD_POS DECODER  (BYTE -> INTEGER)
char commandArray[3];
char rS=0;
byte rH=0;//HIGH BYTE
byte rL=0;//LOW BYTE
byte rF=0;
int cmd0Pos=0; //degree


//ENC STATE
int pinAState = 0;int pinAStateOld = 0;int pinBState = 0;int pinBStateOld = 0;

double angle0Pos = 0;
volatile long encoder0Pos = 0;
volatile long unknownvalue = 0;

Metro mainTimer = Metro(10);


void setup() { 
  pinMode(MotorPin0, OUTPUT);
  pinMode(MotorPin1, OUTPUT);

  
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH); // turn on pullup resistor
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH); // turn on pullup resistor
  
  attachInterrupt(0, doEncoder, CHANGE); // encoder pin on interrupt 0 - pin 2
  attachInterrupt(1, doEncoder, CHANGE);
  
  Serial.begin (57600);
  //Serial.println("start"); // a personal quirk
} 

void loop(){
  if (mainTimer.check() == true) { 

  //GET POS_CMD FROM MEGA
   
      rS=(char)Serial.read();
       if(rS=='{'){
         Serial.readBytes(commandArray,3);
         rH=commandArray[0];
         rL=commandArray[1];
         rF=commandArray[2];
         if(rF=='}')         
           cmd0Pos=(rH<<8)+rL; 
   // ACTUALLY NOW WE DONT NEED TO SEND ENC TO MEGA        
	   //Serial.print("  cmd  ");Serial.print(cmd0Pos);
           //Serial.print("  rS  ");Serial.print(rS);
           //Serial.print("  rH  ");Serial.print(rH);
           //Serial.print("  rL  ");Serial.print(rL);
           //Serial.print("  rF  ");Serial.println(rF);
         rS=0;
         
        for(int i = 0; i < 3; ++i){
          commandArray[i] = '\0';
        }
        
       }




    //GET ANGLE
    angle0Pos=-1*encoder0Pos*0.00694;//(double)360/(64*810*1);
    //angle0Pos=(double)((double)(360.0/64)/810);

    //CONTROL STRATERGY
    double cmdPwm=10*(cmd0Pos-angle0Pos);
 
    //SEND CMD TO MOTOR
    int vPlus=0,vMinus=0;
    if(cmdPwm>=0)
    {
      vPlus=cmdPwm;
      vMinus=0;
    }
    else if(cmdPwm<0)
     {
      vPlus=0;
      vMinus=-cmdPwm;
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
 




/////WRITE ENC to mega
/*
    x=encoder0Pos;  
    char xS='{';
    byte xH =highByte(x);
    byte xL =lowByte(x);
    char xF='}'; 
    Serial.print(xS);Serial.write(xH);Serial.write(xL);Serial.write(xF);
*/
    
    }
}

void doEncoder() {
    // encoder0Pos++;
    pinAState = digitalRead(2);
    pinBState = digitalRead(3);

    if (pinAState == 0 && pinBState == 0) {
    if (pinAStateOld == 1 && pinBStateOld == 0) // forward
    encoder0Pos ++;
    if (pinAStateOld == 0 && pinBStateOld == 1) // reverse
    encoder0Pos --;
    if (pinAStateOld == 1 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    if (pinAStateOld == 0 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    }
    if (pinAState == 0 && pinBState == 1) {
    if (pinAStateOld == 0 && pinBStateOld == 0) // forward
    encoder0Pos ++;
    if (pinAStateOld == 1 && pinBStateOld == 1) // reverse
    encoder0Pos --;
    if (pinAStateOld == 1 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    if (pinAStateOld == 0 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    }
    if (pinAState == 1 && pinBState == 1) {
    if (pinAStateOld == 0 && pinBStateOld == 1) // forward
    encoder0Pos ++;
    if (pinAStateOld == 1 && pinBStateOld == 0) // reverse
    encoder0Pos --;
    if (pinAStateOld == 0 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    if (pinAStateOld == 1 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    }

    if (pinAState == 1 && pinBState == 0) {
    if (pinAStateOld == 1 && pinBStateOld == 1) // forward
    encoder0Pos ++;
    if (pinAStateOld == 0 && pinBStateOld == 0) // reverse
    encoder0Pos --;
    if (pinAStateOld == 0 && pinBStateOld == 1) // unknown
    unknownvalue ++;
    if (pinAStateOld == 1 && pinBStateOld == 0) // unknown
    unknownvalue ++;
    }
    pinAStateOld = pinAState;
    pinBStateOld = pinBState;
}


/*
Serial.print("Encoder: "); Serial.print(encoder0Pos); 
Serial.print(" unKnown: ");Serial.print(unknownvalue, DEC); 
Serial.println(""); 
*/
