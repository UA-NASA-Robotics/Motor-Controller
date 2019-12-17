#include "Macro_Handler/Macro_Mgr.h"
#include "Definitions.h"
#include <stddef.h>
// Will hold the function callback of the macros
MacroCallBack_t macroCallBacks[MaxNumMacros];
unsigned char macroCount = 0;
void initMacroMGR()
{
    int i;
    for(i =0; i <MaxNumMacros;i++)
    {
        macroCallBacks[i].ID = 0;
        macroCallBacks[i].callBack = NULL;
        macroCallBacks[i].parameter = 0;
    }
    macroCount = 0;
}
/* 
 *  < bool setMacroCallback(bool(*_callBack)(int), int _parm, unsigned int _macroID) >
 * 
 *  *_callBack: this parameter should be a pointer to a function that 
 *              will return true when the macro has completed
 * 
 *      _parm : this parameter should be passed to the function callback
 *              every time it is called
 * 
 *    _macroID: this parameter should be a unique id that is associated 
 *              with the macro that is being run
 * 
 *      Return: will return true when the function was successfully 
 *              added to the call stack
 */
bool setMacroCallback(PFI _callBack, int _parm, unsigned int _macroID) {
    int i;
    /* If the macro id is already registered and 
     * running we don't want to added it again */
    for (i = 0; i < MaxNumMacros; i++) {
        
        if (macroCallBacks[i].ID == _macroID)
             return 0;
    }
    bool callBackSet = false;
    for (i = 0; i < MaxNumMacros; i++) {
        if (macroCallBacks[i].callBack == NULL) {
            /* Add the Macro function to the callback list */
            macroCallBacks[i].callBack = _callBack;
            macroCallBacks[i].ID = _macroID;
            macroCallBacks[i].parameter = _parm;
            macroCount++;
            /* Setting that the call back was inserted into the list */
            callBackSet = true;
            /* jump out of the for loop */
            break;
        }
    }
    return callBackSet;
}

/* 
 *  < bool runMacros() >
 * 
 *  Return: will return true when any of the macros in the list completes
 */
bool runMacros() {
    bool macroFinished = false;
    int i;
    for (i = 0; i < MaxNumMacros; i++) {
        /* Running the macro and if it returns true (indicating it has completed)
         * it will or it with the macroFinished state */
        if (macroCallBacks[i].callBack != NULL) {
            /* When a Macro returns true remove it from the macro list */
            if ((*(macroCallBacks[i].callBack))(macroCallBacks[i].parameter)) {
                macroFinished |= 1;
                macroCallBacks[i].callBack = NULL;
                macroCallBacks[i].ID = NULL;
                macroCallBacks[i].parameter = NULL;
                macroCount--;
            }
        }

    }
    return macroFinished;
}

/* 
 *  < uint32_t getRunningMacros()  >
 * 
 *  Return: A 32 bit integer is returned with the IDth bit of
 *          macroResult 1 for each of the IDs found in the macro list
 */
#define isMacroXrunning(c) (getRunningMacros() & (1 << c))

int getRunningMacros() {
    int i;
   uint16_t macroResult = 0;
   bool flag = false;
    for (i = 0; i < MaxNumMacros; i++) {
        /* Running the macro and if it returns true (indicating it has completed)
         * it will or it with the macroFinished state */
        if((macroCallBacks[i].callBack) != NULL) {
            /* The IDth bit of macroResult 1 if the ID if found in the list */
            macroResult = macroResult | macroCallBacks[i].ID;
            flag = true;
        }

    }
   if(flag)
   LED4^=1;
    return (int)(macroResult & 0xFFFF);
}
/* 
 *  < void getRunningMacroCount()  >
 * 
 *  Return: returns the number of registered macros that are running
 */
void getRunningMacroCount()
{
    return macroCount;
}