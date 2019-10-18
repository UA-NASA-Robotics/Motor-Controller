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
/* ************************************************************************** */

#ifndef _DIGGING_MACROS_H    /* Guard against multiple inclusion */
#define _DIGGING_MACROS_H
#include <stdbool.h>

#define DIG_SPEED               6000
#define startDrumDig()          setMotorVel(&DrumMotor, DIG_SPEED)
#define stopDrumDig()           setMotorVel(&DrumMotor, 0)
#define INC_DRUM_HEIGHT_TIME    3000
#define FIND_ZERO_SPEED         1000

#define THE_RIGHT_HEIGHT             3250
#define DRUM_GROUND_HEIGHT      2880
#define DRUM_WEIGHT_FULL    150


void plowMacro(void);
int diggingMacro(int startDepth);

void diggingMacro2(bool b);
bool testDrumCapacityMacro();

void filterMaterials(int displaceDistance);

bool setDiggingHeight(unsigned int _height);
void setDrumTravelMode();

int getDrumWeight();
#endif /* _DIGGING_MACROS_H */

/* *****************************************************************************
 End of File
 */
