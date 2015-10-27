# pos_control_uno_mega2560

* HALL SENSOR condition, its value is relative with 1)distance between hall sensor and meganetic cone  2)the direction of megantic cone 

		if(HomeValue>600) 

* State Machine and state command is from ros_topic 
	
		// STATE MACHINE

* In HOME_STATE , joint go back to find home sensor , and stop there. 

		#define HOME_STATE 0

* In CONTROL_STATE, joint know where is the home origin now, and only can move in the working range.
		#define CONTROL_STATE 1

