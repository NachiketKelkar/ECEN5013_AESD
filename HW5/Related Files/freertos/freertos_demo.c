//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2009-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the DK-TM4C129X Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_nvic.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "grlib/grlib.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "drivers/pinout.h"

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "LEDTask.h"
#include "Logger.h"
#include "Temperature.h"
#include "alert.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>FreeRTOS Example (freertos_demo)</h1>
//*****************************************************************************

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
    tContext sContext;

    //
    // A fatal FreeRTOS error was detected, so display an error message.
    //
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
    GrContextForegroundSet(&sContext, ClrRed);
    GrContextBackgroundSet(&sContext, ClrBlack);
    GrContextFontSet(&sContext, g_psFontCm20);
    GrStringDrawCentered(&sContext, "Fatal FreeRTOS error!", -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         (((GrContextDpyHeightGet(&sContext) - 24) / 2) +
                          24), 1);

    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

/* Global variables */
uint32_t g_ui32SysClock;
QueueHandle_t loggerQueue;

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int main(void)
{
    /* setting up global clock */
    uint32_t g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                                     SYSCTL_OSC_MAIN |
                                                     SYSCTL_USE_PLL |
                                                     SYSCTL_CFG_VCO_480), 120000000);

    /* Initialize UART */
    Logger_Init();
    UARTStdioConfig(0, 115200, g_ui32SysClock);

    /* Setup the queues */
    loggerQueue = xQueueCreate(10, sizeof(logger_struct));
    if(loggerQueue == NULL)
    {
        UARTprintf("Setting up logger queue failed\n\r");
    }

    /* Create all the required tasks */
    xTaskCreate(LEDTask, "LED", 256, NULL, 1, NULL);
    xTaskCreate(LoggerTask, "Logger", 256, NULL, 1, NULL);
    xTaskCreate(TemperatureTask, "Temperature", 256, NULL, 1, NULL);
    xTaskCreate(AlertTask, "Alert", 256, NULL, 1, NULL);

    /* Start the scheduler */
    vTaskStartScheduler();
    return 0;
}
