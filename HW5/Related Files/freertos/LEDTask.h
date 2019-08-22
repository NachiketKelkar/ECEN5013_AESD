/*
 * LEDTask.h
 *
 *  Created on: Apr 6, 2019
 *      Author: nachi
 */

#ifndef LEDTASK_H_
#define LEDTASK_H_
#include "portmacro.h"

/* Global structure for time-stamp */
typedef struct
{
    TickType_t sec;
    TickType_t msec;
}timer;

/*
 * Name: LEDTask()
 * Description: Task to toggle the LED at 10HZ and send the message to logger task
 *              with name, toggle-count for logging it to the screen.
 * @param: (void*) - data to be sent when the task starts
 * @return: void
 */
void LEDTask(void *);


/*
 * Name: timestamp()
 * Description: Get the time from the sys-tick convert it to milliseconds and return the
 *              time in seconds and milliseconds.
 * @param: void
 * @return: timer - structure with data seconds and milliseconds
 */
timer timestamp();


/*
 * Name: TicksToMS()
 * Description: This function takes the noof ticks as parameter and returns the milliseconds
 * @param: TickType_t (No of ticks)
 * @return: TickType_t (time in milliseconds)
 */
TickType_t TicksToMS(TickType_t);

#endif /* LEDTASK_H_ */
