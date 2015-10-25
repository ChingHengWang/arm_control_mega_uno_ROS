# pos_control_Attiny84

#### ISSUE : ENCODER GET ERROR
* description - attiny84 seems not fast to get encodr A B interrupt counter,pwm only can be 50, can't higher then 50, or anglepos will has error
* solution - maybe add external oscillator for attiny84


### WIRE
* MOTOR
NAME             bit    physical
MotorPin0        7      6
MotorPin1        8      5

* ENCODER        bit    physical
encoderPinA      0      13
encoderPinB      1      12

* mySerial       bit    physical
RX               10     2
TX               9      3




### CODE

* PID TERM
		int Kp=5;

* divide to 4 part

	//GET CMD FROM MEGA	
		get_cmd_pos();

	//GET ENC	
		double anglePos=get_angle_from_enc();

	//CONTROL STRATERGY	
		double pidTerm=Kp*(cmdPos-anglePos);
 
	//SEND CMD		
		send_cmd_to_motor(pidTerm,50); 

* add mySerialprintInt to send message by mySerial

    //GET CMD FROM MEGA
		get_cmd_pos();

    //GET ENC
		double anglePos=get_angle_from_enc();

    //CONTROL STRATERGY
		double pidTerm=Kp*(cmdPos-anglePos);
 
    //SEND CMD
		send_cmd_to_motor(pidTerm,50); // pwm limit 52 is the fast speed that encoder can couter 
