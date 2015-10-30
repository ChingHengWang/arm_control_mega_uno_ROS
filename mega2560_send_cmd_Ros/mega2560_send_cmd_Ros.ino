#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Metro.h>

ros::NodeHandle nh;
SoftwareSerial mySerial(10, 11); // RX, TX

//cmd Byte to UNO
char commandArray[3];
char cS=0;
byte cH=0;
byte cL=0;
byte cF=0;
int cmd_P=0;
void messageCb(const std_msgs::Int16& msg)
{
    cmd_P=msg.data;
	
}
ros::Subscriber<std_msgs::Int16> sub("cmd_angle",messageCb);
     
Metro mainTimer = Metro(10);

void setup() { 
 nh.initNode();
 nh.subscribe(sub);
 mySerial.begin(57600);

} 

void loop(){
  if (mainTimer.check() == true) {
    ////WRITE CMD to UNO

    char cS='{';
    byte cH =highByte(cmd_P);
    byte cL =lowByte(cmd_P);
    char cF='}'; 
    mySerial.print(cS);mySerial.write(cH);mySerial.write(cL);mySerial.write(cF);

    nh.spinOnce();
   
  }

}

