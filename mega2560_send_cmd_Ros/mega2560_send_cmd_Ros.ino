#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Metro.h>

ros::NodeHandle nh;
SoftwareSerial mySerial_0(10, 11); // RX, TX
SoftwareSerial mySerial_1(12, 13); // RX, TX

//cmd Byte to UNO
char commandArray[3];
char cS=0;
byte cH=0;
byte cL=0;
byte cF=0;
int cmd_P_0=0;
int cmd_P_1=0;

void messageCallBack0(const std_msgs::Int16& msg)
{
    cmd_P_0=msg.data;
	
}
void messageCallBack1(const std_msgs::Int16& msg)
{
    cmd_P_1=msg.data;
	
}


ros::Subscriber<std_msgs::Int16> sub_0("cmd_angle_0",messageCallBack0);
ros::Subscriber<std_msgs::Int16> sub_1("cmd_angle_1",messageCallBack1);
    
Metro mainTimer = Metro(10);

void setup() { 
 nh.initNode();
 nh.subscribe(sub_0);
 nh.subscribe(sub_1);
 mySerial_0.begin(57600);
 mySerial_1.begin(57600);

} 

void loop(){
  if (mainTimer.check() == true) {
    ////WRITE CMD to UNO

    char cS='{';
    byte cH =highByte(cmd_P_0);
    byte cL =lowByte(cmd_P_0);
    char cF='}'; 
    mySerial_0.print(cS);mySerial_0.write(cH);mySerial_0.write(cL);mySerial_0.write(cF);

    nh.spinOnce();
   
  }

}

