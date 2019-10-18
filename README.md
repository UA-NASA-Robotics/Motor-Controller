# ROCKEE_Motor-Controller (Address = 6)



## Description
This repo contains the code for the main control of the all motor initialization and macro control elements having to do with the motors. This repo is NOT exclusive to ROCKEE but is a living system control element for the continued generations of the UA NASA Robotics' robots.



***
## Initializations

During the initialization process the system is both configuring the motors as well as building the motor profiles for use later on. Things like limit switches as well as encoder information will be used for macro development. I is critical that you get the initialization correct other wise wither you will not be using the motors to their fullest potential or destroy them (which has happened!)
The following is some of the initializations that need to happen with the dunkermotors:
* Clear Errors
* Power Disable (DEV_Enable = 0 is required for many of the configurations to be writable)
* Set Feedback as Encoders or EMF
* Encode Resolutions (counts/revolution) 
* Position Following Error (default value used)
* Clear Actual Position
* Number of Poles (used for brushless motors)
* Limit Switches
* Current Limits
* Velocity limits


For information on the Dunker Motors register addressing and other info see: <https://github.com/UA-NASA-Robotics/ROCKEE_Motor-Controller/blob/master/KnowledgeBase.chm>



***
## Macros

The main elements of motor control during a macro are handled by the _ROCKEE_Motor-Controller_ code. The only exception to this is that the Gyro will handle its own control with completing a macro operation. When macros are run the main principal is that there is some sort of feedback loop going on with a destination value that is trying to be approached. With the case of the Dunkermotoren motors (drive system) there is usually a feedback loop with the encoder counts that are stored in the motors themselves until the value is seen to have reached the correct destination.



