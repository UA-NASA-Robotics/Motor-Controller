/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ***************************************************************************/

#ifndef _DRIVING_CONTROL_H    /* Guard against multiple inclusion */
#define _DRIVING_CONTROL_H
void resetDriveStates();
bool drive2Point(int val);
bool driveDist(int _distance);
bool driveDistance(int _distance,int _speed);
bool driveSeperatDistances(int L_distance,int R_distance, int L_speed, int R_speed);
void driveDistanceVariedSpeed(int distance, int speed);

#endif /* _DRIVING_CONTROL_H */

/* ***************************************************************************
 End of File
 */
