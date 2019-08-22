/*
 * alert.c
 *
 *  Created on: Apr 8, 2019
 *      Author: nachi
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "alert.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "LEDTask.h"
#include "Logger.h"
#include "timers.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

extern QueueHandle_t loggerQueue;
uint8_t edge = 0x00;
bool sendAlert = false;

void AlertTask(void *pvParameters)
{
    logger_struct logStruct;

    //UARTprintf("Inside Alert\n\r");
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }

    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
    //UARTprintf("registering handler\n\r");
    GPIOIntRegister(GPIO_PORTE_BASE, PortEHandler);

    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTE_BASE , GPIO_PIN_0);
    //UARTprintf("Interrupts enabled\n\r");

    strcpy(logStruct.source,"alert");
    strcpy(logStruct.message,"Temperature is above threshold");

    TimerHandle_t timer3  = xTimerCreate("MyTimer3", pdMS_TO_TICKS(200), pdTRUE, (void*)1, Timer3Callback);
    BaseType_t return_val = xTimerStart(timer3, pdMS_TO_TICKS(100));
    //UARTprintf("Timer started\n\r");

    while(1)
    {
        if(edge == 0x01 && sendAlert == true)
        {
            sendAlert = false;
            logStruct.timeStampsec  = timestamp().sec;
            logStruct.timeStampmsec = timestamp().msec;

            xQueueSend(loggerQueue, (void*)&logStruct, 500);
        }
    }
}

void PortEHandler(void)
{
    GPIOIntClear(GPIO_PORTE_BASE, GPIO_PIN_0);
    int32_t read_val = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0);
    if(read_val == 0)
    {
        edge = 0x01;
    }
    else if(read_val == 1)
    {
        edge = 0x02;
    }
}


void Timer3Callback(TimerHandle_t xtimer)
{
    sendAlert = true;
}
