/* 
 * File:   Macro_Mgr.h
 * Author: John
 *
 * Created on November 27, 2019, 11:02 AM
 */

#ifndef MACRO_MGR_H
#define	MACRO_MGR_H

#ifdef	__cplusplus
extern "C" {
#endif
    /*******************************************************/
    /****************** # I N C L U D E S ******************/
    /*******************************************************/
#include <stdbool.h>
#include <stdint.h>
    /*******************************************************/
    /****************** # D E F I N E S ********************/
    /*******************************************************/

#define MaxNumMacros 4
    
#define isMacroXrunning(c) (getRunningMacros() & (1 << c))
#define clearMacros()  initMacroMGR()
    /*******************************************************/
    /****************** O B J E C T S **********************/

    /*******************************************************/
    typedef bool (*PFI)(int);
    
    typedef struct {
        PFI callBack;
        unsigned int ID;
        int parameter;
    } MacroCallBack_t;


    /*******************************************************/
    /****************** F U N C T I O N S ******************/
    /*******************************************************/

void initMacroMGR();
    /* 
     *  < bool setMacroCallback(PFI _callBack, int _parm, unsigned int _macroID) >
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
    bool setMacroCallback(PFI _callBack, int _parm, unsigned int _macroID);

    /* 
     *  < bool runMacros() >
     * 
     *  Return: will return true when any of the macros in the list completes
     */
    bool runMacros();

    /* 
     *  < uint32_t getRunningMacros()  >
     * 
     *  Return: A 32 bit integer is returned with the IDth bit of
     *          macroResult 1 for each of the IDs found in the macro list
     */
    int getRunningMacros();


#ifdef	__cplusplus
}
#endif

#endif	/* MACRO_MGR_H */

