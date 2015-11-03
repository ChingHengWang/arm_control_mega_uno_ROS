#include <SoftwareSerial.h>
#include <Metro.h>
SoftwareSerial mySerial(2, 3); // RX, TX


int x=0;

//read Byte from serial
char commandArray[3];
char rS=0;
char rS2=0;

byte rH=0;
byte rL=0;
char rF=0;
char rF2=0;


Metro mainTimer = Metro(10);


void setup() { 
  Serial.begin (57600);
  mySerial.begin (57600);
  Serial.println("start"); // a personal quirk
} 

void loop(){
  if (mainTimer.check() == true) { 
  //Serial.println("check"); // a personal quirk
  //GET ENC
  if (mySerial.available() > 0) {
    int choice=0;
    switch(choice){
      case 0 :
        rS=(char)mySerial.read();
  
         if(rS=='{'){
           rS2=(char)mySerial.read();
           if(rS2==' '){
             mySerial.readBytes(commandArray,4);
             rH=commandArray[0];
             rL=commandArray[1];
             rF=commandArray[2];
             rF2=commandArray[3];
             if(rF==' '&&rF2=='}')         
               x=(rH<<8)+rL; 
             Serial.print("  cmd  ");Serial.print(x);
             Serial.print("  rS  ");Serial.print(rS);
             Serial.print("  rH  ");Serial.print(rH);
             Serial.print("  rL  ");Serial.print(rL);
             Serial.print("  rF2  ");Serial.println(rF2);
             rS=0;
           
             for(int i = 0; i < 4; ++i){
                 commandArray[i] = '\0';
              }     
           }
         }
         break;
     case 1 :
        byte tmp=mySerial.read(); 
        Serial.print("  tmp  ");Serial.println(tmp);  
        break;
      }
      
    }//if myserial available
  }
}



