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

#ifndef _TESTING_FUNCTIONS_H    /* Guard against multiple inclusion */
#define _TESTING_FUNCTIONS_H


#include <stdio.h>
#include <stdbool.h>

void testMotorControlIntelligently(void);
void testArmMotorDigitalInputs(void);
void testActuatorMotor(void);
void testBucketActuator(void);
void testAnalogFeedbackPlowMotor(void);
void testPlowMovement(void);

void testDiggingFunction(void);
void testMotorDistanceCommand(void);
void testPlowingFunction(void);

#endif /* _TESTING_FUNCTIONS_H */

/* *****************************************************************************
 End of File
 */
