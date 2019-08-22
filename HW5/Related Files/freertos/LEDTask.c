/*
 * LEDTask.c
 *
 *  Created on: Apr 6, 2019
 *      Author: nachi
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "Logger.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "LEDTask.h"

extern QueueHandle_t loggerQueue;
volatile bool send_message = false;

/*
 * Name: Timer1Callback()
 * Description: This is the callback function called after timer expiration. This toggles the LED.
 * @param: TimerHandle_t (timer data)
 * @return: void
 */
void Timer1Callback(TimerHandle_t xtimer);


void LEDTask(void *pvParameters)
{
    volatile uint32_t ui32Loop;
    timer time;
    uint32_t count = 0;
    logger_struct logStruct;

    UARTprintf("Inside LED task\n\r");

    /* Enable the peripheral */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    /* Configure pins as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);

    /* Set up timer to toggle LED at regular interval */
    TimerHandle_t timer1 = xTimerCreate("MyTimer1", pdMS_TO_TICKS(50), pdTRUE, (void*)0, Timer1Callback);
    BaseType_t return_val = xTimerStart(timer1, pdMS_TO_TICKS(100));

    strcpy(logStruct.source,"gpio");
    strcpy(logStruct.name,"nachiket");

    while(1)
    {
        if(send_message == true)
        {
            time = timestamp();
            logStruct.timeStampsec  = time.sec;
            logStruct.timeStampmsec = time.msec;
            logStruct.toggleCount = count;
            send_message = false;

            /* Send message to logger */
            xQueueSend(loggerQueue, (void*)&logStruct, 500);
            count ++;
        }
    }
}

void Timer1Callback(TimerHandle_t xtimer)
{
    static bool LEDState = true;
    send_message = true;
    //UARTprintf("Inside Timer\n\r");
    if(LEDState)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        LEDState = false;
    }
    else
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
        LEDState = true;
    }
}


timer timestamp()
{
    timer timer1;
    TickType_t ticks = xTaskGetTickCount();
    TickType_t time_in_ms = TicksToMS(ticks);
    timer1.sec = time_in_ms/1000;
    timer1.msec = time_in_ms - ((time_in_ms/1000)*1000);
    return timer1;
}


TickType_t TicksToMS(TickType_t ticks)
{
    return ((ticks * (TickType_t)1000) / ( TickType_t ) configTICK_RATE_HZ);
}
