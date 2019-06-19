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

#ifndef _PLOW_CONTROL_H    /* Guard against multiple inclusion */
#define _PLOW_CONTROL_H

#include <stdint.h>

#define PLOW_FULL_UP            -4
#define MAXIMUM_ANALOG_POSITION 3600 // verified - ~5000 max
#define MINIMUM_ANALOG_POSITION 100 // verified -4428 min

void initTimersPlow(void);
void zeroPlowDepth(void);
void setPlowDepth(float depthInInches,int plowSpeed);
void doPlowMacro(int distance);
void doPlowMacro2(int distance);


#endif /* _PLOW_CONTROL_H */

/* *****************************************************************************
 End of File
 */
