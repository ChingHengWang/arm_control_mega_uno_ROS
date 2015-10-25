
#include <ros.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float32.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Metro.h>

void messageCb(const std_msgs::Float32& msg)
{
    Serial.println("callback");
}

ros::NodeHandle nh;
ros::Subscriber<std_msgs::Float32> sub("cmd_angle",messageCb);


     
SoftwareSerial mySerial(10, 11); // RX, TX
Metro mainTimer = Metro(10);

void setup() { 
 nh.initNode();
 nh.subscribe(sub);
 mySerial.begin(57600);

} 

void loop(){
  if (mainTimer.check() == true) {

    nh.spinOnce();
   
  }

}

