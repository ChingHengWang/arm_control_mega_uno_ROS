  
#include <SoftwareSerial.h>
#include <Metro.h>
SoftwareSerial mySerial_0(10, 11); // RX, TX
SoftwareSerial mySerial_1(12, 13); // RX, TX

Metro mainTimer = Metro(10);
//cmd Byte to UNO
char commandArray[3];
struct mySerialByte{
  char cS=0;
  byte cH=0;
  byte cL=0;
  byte cF=0; 
  };
mySerialByte mySerialByte_0,mySerialByte_1;

int cmd_P_0=0;
int cmd_P_1=0;
int HomeValue=0;
void setup() { 

 Serial.begin (115200);
 Serial.println("start");                // a personal quirk
 mySerial_0.begin(57600);
 mySerial_1.begin(57600);


} 

void loop(){
  if (mainTimer.check() == true) {
      
	/////WRITE CMD to UNO

  //MOTOR 0
	  mySerialByte_0.cS='{';
	  mySerialByte_0.cH =highByte(cmd_P_0);
	  mySerialByte_0.cL =lowByte(cmd_P_0);
	  mySerialByte_0.cF='}'; 
	  mySerial_0.print(mySerialByte_0.cS);mySerial_0.write(mySerialByte_0.cH);mySerial_0.write(mySerialByte_0.cL);mySerial_0.write(mySerialByte_0.cF);

  //MOTOR 1
    mySerialByte_1.cS='{';
    mySerialByte_1.cH =highByte(cmd_P_1);
    mySerialByte_1.cL =lowByte(cmd_P_1);
    mySerialByte_1.cF='}'; 
    mySerial_1.print(mySerialByte_1.cS);mySerial_1.write(mySerialByte_1.cH);mySerial_1.write(mySerialByte_1.cL);mySerial_1.write(mySerialByte_1.cF);






   
    char rS=(char)mySerial_1.read();
    if(rS=='{'){
    char commandArray[3];
    mySerial_1.readBytes(commandArray,3);
    byte rH=commandArray[0];
    byte rL=commandArray[1];
    char rF=commandArray[2];
    if(rF=='}')         
      HomeValue=(rH<<8)+rL; 
    }
    Serial.println(HomeValue);
  }

}






