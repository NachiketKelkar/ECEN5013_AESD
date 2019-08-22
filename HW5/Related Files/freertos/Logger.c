/*
 * Logger.c
 *
 *  Created on: Apr 6, 2019
 *      Author: nachi
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "Logger.h"
#include "utils/uartstdio.h"
#include "projdefs.h"

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t loggerQueue;

void Logger_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}


void Logger_printf(logger_struct logStruct)
{
    UARTprintf("[%d sec %03d msec]", logStruct.timeStampsec, logStruct.timeStampmsec);
    UARTprintf("[%s]",logStruct.source);
    /* If message is from the alert task */
    if(strcmp(logStruct.source,"alert") == 0)
    {
        UARTprintf("%s\n\r",logStruct.message);
    }
    /* If message is from the temperature task */
    else if(strcmp(logStruct.source,"temperature") == 0)
    {
        char printit[30];
        sprintf(printit,"Temperature is %f",logStruct.temperature);
        UARTprintf("%s\n\r",printit);
    }
    /* If message is from the gpio task */
    else if(strcmp(logStruct.source,"gpio") == 0)
    {
        UARTprintf("Toggle count is %d",logStruct.toggleCount);
        UARTprintf("    Your name is %s\n\r",logStruct.name);
    }
}


void LoggerTask(void *pvParameters)
{
    UARTprintf("Inside LoggerTask\n\r");
    logger_struct data_received;

    while(1)
    {
        if(xQueueReceive(loggerQueue, &data_received, 10000) == pdPASS)
        {
            Logger_printf(data_received);
        }
    }
}
