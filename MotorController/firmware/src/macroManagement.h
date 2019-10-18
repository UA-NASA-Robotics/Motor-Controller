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

#ifndef _MACRO_MANAGEMENT_H    /* Guard against multiple inclusion */
#define _MACRO_MANAGEMENT_H

#include <stdbool.h>
#include <stdint.h>

bool continueMacro(void);
void setContinueMacroState(bool state);
void initMacroCommunications(void);
void processMacro(uint8_t performMacro, int macroData);
void updateMacroCommunications(void);


#endif /* _MACRO_MANAGEMENT_H */

/* *****************************************************************************
 End of File
 */
