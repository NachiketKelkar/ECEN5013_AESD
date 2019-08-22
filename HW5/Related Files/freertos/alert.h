/*
 * alert.h
 *
 *  Created on: Apr 8, 2019
 *      Author: nachi
 */

#ifndef ALERT_H_
#define ALERT_H_

/*
 * Name: AlertTask()
 * Description: The task which setup GPIO interrupt and is notified by the interrupt when
 *              the temperature is above threshold.
 * @param: void* (Parameter to be passed to the task while its creation)
 * @return: void
 */
void AlertTask(void *);


/*
 * Name: Timer3Callback()
 * Description: This is timer callback function which sets the global variable which indicates
 *              that if the timer is expired and the temperature above threshold then send the
 *              error to the logger
 * @param: TimerHandle_t (timer data)
 * @return: void
 */
void Timer3Callback();


/*
 * Name: PortEHandler()
 * Description: This function is the callback function for the interrupt on the PortE pin 0.
 *              It the interrupt handler.
 * @param: void
 * @return: void
 */
void PortEHandler(void);

#endif /* ALERT_H_ */
