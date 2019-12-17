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
    
#ifndef GBL_DEV_IDX
#define GBL_DEV_IDX
//Complete index of all the
typedef enum {
	DEVICE_STATUS=0,
	DEVICE_MACRO,
	DATA_0,
	DATA_1,
	DATA_2,
	DATA_3,
	DATA_4
} GlobalDeviceData_t;
#endif

    /** \brief (initGlobalData): Initializes the data publishing values to be put on the can bus at a particular interval
     *
     * \param (_interval):  what is the index in the "memory" that is sent out
     * \param (getFuncPointer):  A pointer to the function that will return the data 
     *                           that you wish to get the data from
     * \param (_interval):  The interval you wish to publish the data on the can buss
     * \return: N/A
     */
    void initGlobalData(GlobalDeviceData_t _index, int (*getFuncPointer)(void), unsigned long _interval);



    /** \brief: Publishes the data if and only if the timers for the specific data elements has expired
     *
     * \return: True if data was published
     */
    bool publishData();



#ifdef	__cplusplus
}
#endif

#endif	/* DATAPUBLISHING_H */

