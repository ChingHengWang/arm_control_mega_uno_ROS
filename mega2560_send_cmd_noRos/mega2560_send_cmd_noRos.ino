  
#include <SoftwareSerial.h>
#include <Metro.h>
SoftwareSerial mySerial(10, 11); // RX, TX
Metro mainTimer = Metro(10);
//cmd Byte to UNO
char commandArray[3];
char cS=0;
byte cH=0;
byte cL=0;
byte cF=0;
int cmd_P=200;
int HomeValue=0;
void setup() { 

 Serial.begin (115200);
 Serial.println("start");                // a personal quirk
 mySerial.begin(57600);


} 

void loop(){
  if (mainTimer.check() == true) {
      
	/////WRITE CMD to UNO

	  char cS='{';
	  byte cH =highByte(cmd_P);
	  byte cL =lowByte(cmd_P);
	  char cF='}'; 
	  mySerial.print(cS);mySerial.write(cH);mySerial.write(cL);mySerial.write(cF);

   
    char rS=(char)mySerial.read();
    if(rS=='{'){
    char commandArray[3];
    mySerial.readBytes(commandArray,3);
    byte rH=commandArray[0];
    byte rL=commandArray[1];
    char rF=commandArray[2];
    if(rF=='}')         
      HomeValue=(rH<<8)+rL; 
    }
    Serial.println(HomeValue);
  }

}






