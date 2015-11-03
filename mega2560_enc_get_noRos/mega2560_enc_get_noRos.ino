
#include <SoftwareSerial.h>
#include <Metro.h>
SoftwareSerial mySerial(13, 9); // RX, TX
Metro mainTimer = Metro(10);
boolean stringComplete = false;
char commandArray[3];
char xS=0,xS1=0,xS2=0;
byte xH=0;
byte xL=0;
char xF=0;
int v=0;
void setup() { 

 Serial.begin (115200);
 Serial.println("start");                // a personal quirk
 mySerial.begin(57600);


} 

void loop(){

  if (mainTimer.check() == true) {

    if (mySerial.available()>0) {
   
      xS=(char)mySerial.read();
       if(xS=='{'){
         mySerial.readBytes(commandArray,3);
         xH=commandArray[0];
         xL=commandArray[1];
         xF=commandArray[2];
         if(xF=='}')         
           v=(xH<<8)+xL; 
         Serial.print("  v  ");Serial.print(v);
         Serial.print("  xS  ");Serial.print(xS);
         Serial.print("  xH  ");Serial.print(xH);
         Serial.print("  xL  ");Serial.print(xL);
         Serial.print("  xF  ");Serial.println(xF);
         xS=0;
         
        for(int i = 0; i < 3; ++i){
    commandArray[i] = '\0';
        }
        
       }
      }


  }

}




