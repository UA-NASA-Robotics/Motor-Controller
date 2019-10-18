/* 
 * File:   ArcDriveMacro.h
 * Author: John
 *
 * Created on March 8, 2019, 1:58 PM
 */

#ifndef ARCDRIVEMACRO_H
#define	ARCDRIVEMACRO_H

#ifdef	__cplusplus
extern "C" {
#endif
typedef struct{
    double x;
    double y;
}point_t;

typedef struct {
    point_t StartPoint;
    point_t EndPoint;
    double InitialHeading;
    double FinalHeading;
    double ArcLength;
    double ArcAngle;
    double ArcRadius;
}Arc_t;

void ArcDrive(int _current_x, int _current_y, int _end_x, int _end_y, int _heading);

#ifdef	__cplusplus
}
#endif

#endif	/* ARCDRIVEMACRO_H */

