#include "ArcDriveMacro.h"
#include "motorHandler.h"
#include "drivingControl.h"
#include <math.h>
#define pi 3.14159
#define DEG_TO_RAD  0.017453292

#define TREAD_BASE_WIDTH    50      //centimeters
#define TREAD_SPEED_SCALER  35
Arc_t thisArc;
void CalculateArc();
void ArcDrive(int _current_x, int _current_y, int _end_x, int _end_y, int _heading)
{
    thisArc.StartPoint = (point_t){_current_x, _current_y};
    thisArc.EndPoint = (point_t){_end_x, _end_y};
    thisArc.InitialHeading = _heading;
    CalculateArc();
    int Lspeed,Rspeed;
    int Ldist,Rdist;
    if(thisArc.ArcRadius > 0 ){
        Ldist = (thisArc.ArcRadius - (TREAD_BASE_WIDTH/2)) *thisArc.ArcAngle;
        Rdist = (thisArc.ArcRadius + (TREAD_BASE_WIDTH/2)) *thisArc.ArcAngle;
        Lspeed = Ldist*(TREAD_SPEED_SCALER)-1000;
        Rspeed = Rdist*TREAD_SPEED_SCALER;
    }else{
        Ldist = (thisArc.ArcRadius + (TREAD_BASE_WIDTH/2)) *thisArc.ArcAngle;
        Rdist = (thisArc.ArcRadius - (TREAD_BASE_WIDTH/2)) *thisArc.ArcAngle;
         Lspeed = Ldist*TREAD_SPEED_SCALER;
        Rspeed = Rdist*TREAD_SPEED_SCALER;
    }
   
    driveSeperatDistances(Ldist,Rdist,Lspeed,Rspeed);
}

void CalculateArc()//point_t endPoint, double heading)
{
    /* Math for this was found on http://rossum.sourceforge.net/papers/CalculationsForRobotics/CirclePath.htm */
    double rg,Og;
    //turningArc.EndPoint = endPoint;

    Og = atan((thisArc.EndPoint.x - thisArc.StartPoint.x)/(thisArc.EndPoint.y - thisArc.StartPoint.y));
    Og -= thisArc.InitialHeading*DEG_TO_RAD;
    rg = (thisArc.EndPoint.x - thisArc.StartPoint.x)/cos(Og);

    thisArc.ArcAngle = 2*Og;

    if(Og > pi/2){thisArc.ArcAngle = 2*(Og - pi);}
    else if (Og < -pi/2){thisArc.ArcAngle = 2*(Og + pi);}
    else { thisArc.ArcAngle = 2*Og;}
    thisArc.ArcRadius = rg/(2*sin(Og));
    thisArc.ArcLength = thisArc.ArcRadius*thisArc.ArcAngle;
    
}
