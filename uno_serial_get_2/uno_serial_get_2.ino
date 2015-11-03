#include <SoftwareSerial.h>
#include <Metro.h>
SoftwareSerial mySerial(2, 3); // RX, TX


int x=0;
int state=0;
//read Byte from serial
char commandArray[3];
char rS=0;
byte rSH=0;
byte rSL=0;
byte rPH=0;
byte rPL=0;
char rF=0;

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
           mySerial.readBytes(commandArray,5);
           rSH=commandArray[0];
           rSL=commandArray[1];
           rPH=commandArray[2];
           rPL=commandArray[3];           
           rF=commandArray[4];
           if(rF=='}') {  
             x=(rPH<<8)+rPL;      
             state=(rSH<<8)+rSL;
           Serial.print("  cmd  ");Serial.print(x);
           Serial.print("  state  ");Serial.print(state);
           Serial.print("  rS  ");Serial.print(rS);
           Serial.print("  rSH  ");Serial.print(rSH);
           Serial.print("  rSL  ");Serial.print(rSL);
           Serial.print("  rPH  ");Serial.print(rPH);
           Serial.print("  rPL  ");Serial.print(rPL);
           Serial.print("  rF  ");Serial.println(rF);
           rS=0;
           }
          for(int i = 0; i < 3; ++i){
      commandArray[i] = '\0';
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



