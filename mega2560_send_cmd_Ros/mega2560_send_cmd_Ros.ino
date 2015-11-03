#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int16.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Metro.h>
#define ARM_RIGHT_EN 1


 
ros::NodeHandle nh;
SoftwareSerial mySerial_0(10, 6); // RX, TX
SoftwareSerial mySerial_1(11, 7); // RX, TX
SoftwareSerial mySerial_2(12, 8); // RX, TX
SoftwareSerial mySerial_3(13, 9); // RX, TX

//cmd Byte to UNO
char commandArray[3];
struct mySerialByte{
  char cS=0;
  byte cSH=0;
  byte cSL=0;
  byte cPH=0;
  byte cPL=0;
  byte cF=0; 
  };
mySerialByte mySerialByte_0,mySerialByte_1,mySerialByte_2,mySerialByte_3;



double tick=(double)6.28/32767; //resolution

int cmd_P_0=0;
int cmd_P_1=0;
int cmd_P_2=0;
int cmd_P_3=0;

int home_0=0;
int home_1=0;
int home_2=0;
int home_3=0;

void messageCallBackHome0(const std_msgs::Int16& msg)
{
    home_0=msg.data;
  
}

void messageCallBackHome1(const std_msgs::Int16& msg)
{
    home_1=msg.data;
  
}

void messageCallBackHome2(const std_msgs::Int16& msg)
{
    home_2=msg.data;
  
}

void messageCallBackHome3(const std_msgs::Int16& msg)
{
    home_3=msg.data;
  
}

void messageCallBack0(const std_msgs::Float64& msg)
{
    cmd_P_0=(int)(msg.data/tick);
  
}
void messageCallBack1(const std_msgs::Float64& msg)
{
    cmd_P_1=(int)(msg.data/tick);
  
}
void messageCallBack2(const std_msgs::Float64& msg)
{
    cmd_P_2=(int)(msg.data/tick);
  
}
void messageCallBack3(const std_msgs::Float64& msg)
{
    cmd_P_3=(int)(msg.data/tick);
  
}

#if(ARM_RIGHT_EN)
  ros::Subscriber<std_msgs::Int16> sub_home_0("andbot/joint/R0/cmd/home",messageCallBackHome0);
  ros::Subscriber<std_msgs::Int16> sub_home_1("andbot/joint/R1/cmd/home",messageCallBackHome1);
  ros::Subscriber<std_msgs::Int16> sub_home_2("andbot/joint/R2/cmd/home",messageCallBackHome2);
  ros::Subscriber<std_msgs::Int16> sub_home_3("andbot/joint/R3/cmd/home",messageCallBackHome3);
  ros::Subscriber<std_msgs::Float64> sub_0("andbot/joint/R0/cmd/position",messageCallBack0);
  ros::Subscriber<std_msgs::Float64> sub_1("andbot/joint/R1/cmd/position",messageCallBack1);
  ros::Subscriber<std_msgs::Float64> sub_2("andbot/joint/R2/cmd/position",messageCallBack2);
  ros::Subscriber<std_msgs::Float64> sub_3("andbot/joint/R3/cmd/position",messageCallBack3);  
#else
  ros::Subscriber<std_msgs::Int16> sub_home_0("andbot/joint/L0/cmd/home",messageCallBackHome0);
  ros::Subscriber<std_msgs::Int16> sub_home_1("andbot/joint/L1/cmd/home",messageCallBackHome1);
  ros::Subscriber<std_msgs::Int16> sub_home_2("andbot/joint/L2/cmd/home",messageCallBackHome2);
  ros::Subscriber<std_msgs::Int16> sub_home_3("andbot/joint/L3/cmd/home",messageCallBackHome3);
  ros::Subscriber<std_msgs::Float64> sub_0("andbot/joint/L0/cmd/position",messageCallBack0);
  ros::Subscriber<std_msgs::Float64> sub_1("andbot/joint/L1/cmd/position",messageCallBack1);
  ros::Subscriber<std_msgs::Float64> sub_2("andbot/joint/L2/cmd/position",messageCallBack2);
  ros::Subscriber<std_msgs::Float64> sub_3("andbot/joint/L3/cmd/position",messageCallBack3);      
#endif

Metro mainTimer = Metro(10);

void setup() { 
 nh.initNode();
 nh.subscribe(sub_home_0);
 nh.subscribe(sub_home_1);
 nh.subscribe(sub_home_2);
 nh.subscribe(sub_home_3);
 nh.subscribe(sub_0);
 nh.subscribe(sub_1);
 nh.subscribe(sub_2);
 nh.subscribe(sub_3);
 Serial.begin(57600);
 mySerial_0.begin(57600);
 mySerial_1.begin(57600);
 mySerial_2.begin(57600);
 mySerial_3.begin(57600);

} 

void loop(){
  if (mainTimer.check() == true) {
    ////WRITE CMD to UNO
    mySerialByte_0.cS='{';
    mySerialByte_0.cSH =highByte(home_0);
    mySerialByte_0.cSL =lowByte(home_0);
    mySerialByte_0.cPH =highByte(cmd_P_0);
    mySerialByte_0.cPL =lowByte(cmd_P_0);
    mySerialByte_0.cF='}'; 
    mySerial_0.print(mySerialByte_0.cS);mySerial_0.write(mySerialByte_0.cSH);mySerial_0.write(mySerialByte_0.cSL);mySerial_0.write(mySerialByte_0.cPH);mySerial_0.write(mySerialByte_0.cPL);mySerial_0.write(mySerialByte_0.cF);

    mySerialByte_1.cS='{';
    mySerialByte_1.cSH =highByte(home_1);
    mySerialByte_1.cSL =lowByte(home_1);
    mySerialByte_1.cPH =highByte(cmd_P_1);
    mySerialByte_1.cPL =lowByte(cmd_P_1);
    mySerialByte_1.cF='}'; 
    mySerial_1.print(mySerialByte_1.cS);mySerial_1.write(mySerialByte_1.cSH);mySerial_1.write(mySerialByte_1.cSL);mySerial_1.write(mySerialByte_1.cPH);mySerial_1.write(mySerialByte_1.cPL);mySerial_1.write(mySerialByte_1.cF);

    
    mySerialByte_2.cS='{';
    mySerialByte_2.cSH =highByte(home_2);
    mySerialByte_2.cSL =lowByte(home_2);
    mySerialByte_2.cPH =highByte(cmd_P_2);
    mySerialByte_2.cPL =lowByte(cmd_P_2);
    mySerialByte_2.cF='}'; 
    mySerial_2.print(mySerialByte_2.cS);mySerial_2.write(mySerialByte_2.cSH);mySerial_2.write(mySerialByte_2.cSL);mySerial_2.write(mySerialByte_2.cPH);mySerial_2.write(mySerialByte_2.cPL);mySerial_2.write(mySerialByte_2.cF);

    mySerialByte_3.cS='{';
    mySerialByte_3.cSH =highByte(home_3);
    mySerialByte_3.cSL =lowByte(home_3);
    mySerialByte_3.cPH =highByte(cmd_P_3);
    mySerialByte_3.cPL =lowByte(cmd_P_3);
    mySerialByte_3.cF='}'; 
    mySerial_3.print(mySerialByte_3.cS);mySerial_3.write(mySerialByte_3.cSH);mySerial_3.write(mySerialByte_3.cSL);mySerial_3.write(mySerialByte_3.cPH);mySerial_3.write(mySerialByte_3.cPL);mySerial_3.write(mySerialByte_3.cF);
    nh.spinOnce();
   
    if (mySerial_3.available()>0) 
      get_cmd_pos_3();
  }

}



void get_cmd_pos_3() {

    //GET POS_CMD FROM MEGA
     int x=0;
     byte rS=mySerial_3.read();  
     if(rS=='{'){
     char receiveArray[3];
     mySerial_3.readBytes(receiveArray,3);
     byte rH=receiveArray[0];
     byte rL=receiveArray[1];
     char rF=receiveArray[2];
     if(rF=='}') {        
       x=(rH<<8)+rL; 
     Serial.print("  x  ");Serial.print(x);
     Serial.print("  rS  ");Serial.print(rS);
     Serial.print("  rH  ");Serial.print(rH);
     Serial.print("  rL  ");Serial.print(rL);
     Serial.print("  rF  ");Serial.println(rF);
     }
    }

}


