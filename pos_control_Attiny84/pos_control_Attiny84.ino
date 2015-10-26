
#include <SoftwareSerial.h>
#include <Metro.h>


//MOTOR PWM PIN ASSIGNMENT
#define MotorPin0 7
#define MotorPin1 8


//ENCODER PIN ASSIGNMENT
#define encoderPinA 0
#define encoderPinB 1

SoftwareSerial mySerial(10, 9); //physical Rx 2  Tx 3

int cmdPos=50; //degree

//OLD VERSION ENC STATE
int pinAState = 0;int pinAStateOld = 0;int pinBState = 0;int pinBStateOld = 0;
volatile long unknownvalue = 0;
// ENC STATE
volatile long encoderPos = 0;
volatile long lastEncoder = 0;

//PID TERM
int Kp=5;

Metro mainTimer = Metro(10);


void setup() { 

    pinMode(MotorPin0, OUTPUT);
    pinMode(MotorPin1, OUTPUT);

    pinMode(encoderPinA, INPUT); 
    digitalWrite(encoderPinA, HIGH); // turn on pullup resistor
    pinMode(encoderPinB, INPUT); 
    digitalWrite(encoderPinB, HIGH); // turn on pullup resistor


    mySerial.begin(19200);
    GIMSK  = 0b00110000; //Pin Change Interrupt Enable PCIF0 PCIF10
    PCMSK0 = 0b00000011; //turn on interrupt for PCINT0, PCINT1
    PCMSK1 = 0b00000011; //turn on interrupt for PCINT8, PCINT9


} 

void loop(){
  if (mainTimer.check() == true) {


    //GET CMD FROM MEGA
    get_cmd_pos();

    //GET ENC
    double anglePos=get_angle_from_enc();

    //CONTROL STRATERGY
    double pidTerm=Kp*(cmdPos-anglePos);
 
    //SEND CMD
    send_cmd_to_motor(pidTerm,150); // pwm limit 52 is the fast speed that encoder can couter 
    mySerialprintInt((int)anglePos);
    

    }
}



void mySerialprintInt(int v) {

    //SEND MSG BACK TO mySerial
    char cS='{';
    byte cH =highByte(int(v));
    byte cL =lowByte(int(v));
    char cF='}';
    mySerial.print(cS);mySerial.write(cH);mySerial.write(cL);mySerial.write(cF);
    
}

void mySerialprintint(int v) {

    //SEND MSG BACK TO mySerial
    mySerial.print(v);
    
}

void get_cmd_pos() {

    //GET POS_CMD FROM MEGA

    char rS=(char)mySerial.read();
    if(rS=='{'){
    char commandArray[3];
    mySerial.readBytes(commandArray,3);
    byte rH=commandArray[0];
    byte rL=commandArray[1];
    char rF=commandArray[2];
    if(rF=='}')         
      cmdPos=(rH<<8)+rL; 
    // ACTUALLY NOW WE DONT NEED TO SEND ENC TO MEGA        
    //mySerial.print("  cmd  ");Serial.print(cmd0Pos);
    //mySerial.print("  rS  ");Serial.print(rS);
    //mySerial.print("  rH  ");Serial.print(rH);
    //mySerial.print("  rL  ");Serial.print(rL);
    //mySerial.print("  rF  ");Serial.println(rF);

    }

}


double get_angle_from_enc() {

    double result=1*encoderPos*0.00694;//(double)360/(64*810*1);
    return result;
}



void send_cmd_to_motor(int cmdpwm,int PWM_LIMIT) {

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
    
    if(vPlus>=PWM_LIMIT)
        vPlus=PWM_LIMIT;
    else if(vPlus<-PWM_LIMIT)
        vPlus=-PWM_LIMIT;
          
          
    if(vMinus>=PWM_LIMIT)
        vMinus=PWM_LIMIT;
    else if(vMinus<-PWM_LIMIT)
        vMinus=-PWM_LIMIT;      
    analogWrite(MotorPin1,vPlus);
    analogWrite(MotorPin0,vMinus);
 
}


ISR(PCINT0_vect)
{

    //int MSB = digitalRead(encoderPinA); //MSB = most significant bit
    //int LSB = digitalRead(encoderPinB); //LSB = least significant bit
    byte encoder = (digitalRead(encoderPinA) << 1) | digitalRead(encoderPinB); //converting the 2 pin value to single number
    byte sum  = (lastEncoder << 2) | encoder; //adding it to the previous encoded value
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        encoderPos++;
    else if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        encoderPos--;
    else
        unknownvalue++;  
    lastEncoder = encoder; //store this value for next time
 
    

  
}

