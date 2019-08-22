/*
 * Logger.h
 *
 *  Created on: Apr 6, 2019
 *      Author: nachi
 */

#ifndef LOGGER_H_
#define LOGGER_H_

/*
 * Name: LoggerTask()
 * Description: This function is the logger task which readds the messages in the queues
 *              and logs it to the screen.
 * @param: void
 * @return: void
 */
void LoggerTask(void*);


/*
 * Name: Logger_Init()
 * Description: This function initializes the logger i.e the UART
 * @param: void
 * @return: void
 */
void Logger_Init(void);

typedef struct{
    char source[10];
    uint32_t timeStampsec;
    uint32_t timeStampmsec;
    uint32_t toggleCount;
    float temperature;
    char name[20];
    char message[30];
}logger_struct;


/*
 * Name: Logger_printf()
 * Description: This function logs the readings to the UART
 * @param: logger_struct (as mentioned above)
 * @return: void
 */
void Logger_printf(logger_struct);
#endif /* LOGGER_H_ */
