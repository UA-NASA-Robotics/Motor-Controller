/* 
 * File:   DataPublishing.h
 * Author: Seth Carpenter
 *
 * Created on September 27, 2019, 9:18 PM
 */

#ifndef DATAPUBLISHING_H
#define	DATAPUBLISHING_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdbool.h>

    typedef enum {
        Init_Element = 0,
        Element_1,
        Element_2,
        Element_3,
        Element_4,
        Element_5,
        Element_6,
    } GLOBL_dataElement_t;
    /** \brief (initGlobalData): Initializes the data publishing values to be put on the can bus at a particular interval
     *
     * \param (_interval):  what is the index in the "memory" that is sent out
     * \param (getFuncPointer):  A pointer to the function that will return the data 
     *                           that you wish to get the data from
     * \param (_interval):  The interval you wish to publish the data on the can buss
     * \return: N/A
     */
    void initGlobalData(GLOBL_dataElement_t _index, int (*getFuncPointer)(void), unsigned long _interval);



    /** \brief: Publishes the data if and only if the timers for the specific data elements has expired
     *
     * \return: True if data was published
     */
    bool publishData();



#ifdef	__cplusplus
}
#endif

#endif	/* DATAPUBLISHING_H */

