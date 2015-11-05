# ARMS_Rostopic_Format.md

# ROS Topic Type

## Command position
* rostopic name 
    * ***/andbot/joints/[LR][0:3]/cmd/position***
* message type 
    * ***std_msgs/Float64***
* value range and unit
    * range : 0 to max wroking range 
    * unit: radius

## Command form back home
* rostopic name 
    * ***/andbot/joints/[LR][0:3]/cmd/home***
* message type 
    * ***std_msgs/Int16***
* value 
    * 0 : uneable back to home event
    * 1 : enable back to home event , can enable this event in every status


## feedback for present angle position 
* rostopic name
    * ***/andbot/joints/[LR][0:3]/feedback/angle***
* message type
    * ***std_msgs/Float64***
* value range and unit
    * range : 0 to max wroking range 
    * unit: ***radius***

## feedback for present joint status
* rostopic name
    * ***/andbot/joints/[LR][0:3]/feedbacd/status
* message type
    * ***std_msgs/Int16***
* value 
    * 0 : INIT_STATE
    * 1 : HOME_STATE
    * 2 : HOME_ZONE_STATE  
    * 3 : WORKING_RANGE_STATE
    * 4 : END_ZONE_STATE


