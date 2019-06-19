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

#ifndef _ACTUATOR_CONTROL_H    /* Guard against multiple inclusion */
#define _ACTUATOR_CONTROL_H

#include <stdint.h>

void zeroActuator(void);
void moveActuatorToCounts(int counts, int speed);
void sendActuatorToDig(int speed);
void sendActuatorToDump(int speed);
void moveActuatorToLevelGround(int speed);
void moveActuatorToFullDigging(int speed);
void moveActuatorThroughDig(int speed);
void moveActuatorToDump(int speed);
void twerkIt(void);
void popLockAndDropIt(void);
void diggingActuatorResetPositionData(void);
void diggingActuatorRequestPositionData(void);
void diggingActuatorRequestDigitalData(void);
void diggingActuatorContinueOperation(void);

#endif /* _ACTUATOR_CONTROL_H */

/* *****************************************************************************
 End of File
 */
