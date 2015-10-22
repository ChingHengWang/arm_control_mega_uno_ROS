
#include <ros.h>
#include <geometry_msgs/Vector3.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float64.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Metro.h>
#include <stdio.h>
ros::NodeHandle nh;
std_msgs::Int16 int_msg;
ros::Publisher debug_1("DEBUG_1", &int_msg);
std_msgs::String str_msg;
ros::Publisher debug_2("DEBUG_2", &str_msg);
     
SoftwareSerial mySerial(10, 11); // RX, TX
Metro mainTimer = Metro(10);
char commandArray[3];
char xS=0;
byte xH=0;
byte xL=0;
byte xF=0;
int v=0;

void setup() { 
 nh.initNode();
 nh.advertise(debug_1);
 mySerial.begin(57600);

} 

void loop(){

  if (mainTimer.check() == true) {

    if (mySerial.available()>0) {
   

       if(xS=='S'){
         mySerial.readBytes(commandArray,3);
         xH=commandArray[0];
         xL=commandArray[1];
         xF=commandArray[2];         
         v=(xH<<8)+xL; 

        for(int i = 0; i < 3; ++i){
    commandArray[i] = '\0';
        }
       }
      }

          int_msg.data="v";
            debug_1.publish(&int_msg);


    nh.spinOnce();
   
  }

}

