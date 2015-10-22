
#include <SoftwareSerial.h>
#include <Metro.h>
SoftwareSerial mySerial(10, 11); // RX, TX
Metro mainTimer = Metro(10);
boolean stringComplete = false;
char commandArray[3];
char xS=0;
byte xH=0;
byte xL=0;
byte xF=0;
int v=0;
void setup() { 

 Serial.begin (115200);
 Serial.println("start");                // a personal quirk
 mySerial.begin(57600);


} 

void loop(){
// do some stuff here - the joy of interrupts is that they take care of themselves
  if (mainTimer.check() == true) {

  if (mySerial.available()>0) {
    
     xS=(char)mySerial.read();
     if(xS=='S'){
      
       mySerial.readBytes(commandArray,3);
       xH=commandArray[0];
       xL=commandArray[1];
       xF=commandArray[2];         
       v=(xH<<8)+xL; 
      Serial.print("v ");Serial.println(v);
    
      for(int i = 0; i < 3; ++i){
        commandArray[i] = '\0';
      }
     }
     
    }
  }

/* 
    int xH7=bitRead(xH,7);
    int xH6=bitRead(xH,6);
    int xH5=bitRead(xH,5);
    int xH4=bitRead(xH,4);
    int xH3=bitRead(xH,3);
    int xH2=bitRead(xH,2);
    int xH1=bitRead(xH,1);
    int xH0=bitRead(xH,0); 
    int v=~xH7<<7+xH6<<6+xH5<<5+xH4<<4+xH3<<3+xH2<<2+xH1<<1+xH0<<0;    
*/   

}




