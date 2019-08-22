/*
 * Temperature.c
 *
 *  Created on: Apr 6, 2019
 *      Author: nachi
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "projdefs.h"
#include "portmacro.h"
#include "utils/uartstdio.h"

#include "Temperature.h"
#include "Logger.h"
#include "LEDTask.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"

extern uint32_t g_ui32SysClock;

bool measureTemperature = false;
extern QueueHandle_t loggerQueue;

void TemperatureTask(void *pvParameters)
{
    UARTprintf("Inside Temperature Sensor task\n\r");
    logger_struct logStruct;

    /* Initialize the i2c */
    i2c_init();
    configure_i2c_master();

    /* Setup timer to task temperature readings at regular intervals */
    TimerHandle_t timer2  = xTimerCreate("MyTimer2", pdMS_TO_TICKS(1000), pdTRUE, (void*)1, Timer2Callback);
    if(timer2 == NULL)
    {
        UARTprintf("Timer 2 init failed\n\r");
    }

    BaseType_t return_val = xTimerStart(timer2, pdMS_TO_TICKS(100));
    if(return_val != pdPASS)
    {
        UARTprintf("Failed starting timer 2\n\r");
    }

    /* Configure the registers */
    i2c_write(TEMP_SENS_ADDR, TLOW_REG, 27);
    /*float tlow_val  = read_temperature(TEMP_SENS_ADDR, TLOW_REG);
    char printit[30];
    sprintf(printit,"TLOW reg = %f",tlow_val);
    UARTprintf("%s",printit);*/

    i2c_write(TEMP_SENS_ADDR, THIGH_REG, 27);
    /*float thigh_val  = read_temperature(TEMP_SENS_ADDR, THIGH_REG);
    sprintf(printit,"THIGH reg = %f",thigh_val);
    UARTprintf("%s",printit);*/

    strcpy(logStruct.source,"temperature");
    while(1)
    {
        if(measureTemperature == true)
        {
            measureTemperature = false;
            /* Take the temperature readings */
            taskENTER_CRITICAL();
            float temperature = read_temperature(TEMP_SENS_ADDR, TEMP_REG);
            taskEXIT_CRITICAL();
            logStruct.temperature = temperature;
            logStruct.timeStampsec  = timestamp().sec;
            logStruct.timeStampmsec = timestamp().msec;

            xQueueSend(loggerQueue, (void*)&logStruct, 500);
        }
    }
}


void i2c_init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
}


uint16_t i2c_read(uint8_t slave_addr, uint8_t reg)
{
    uint32_t data1, data2;

    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
    I2CMasterDataPut(I2C0_BASE, reg);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(I2C0_BASE));

    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(I2CMasterBusy(I2C0_BASE));

    data1 = I2CMasterDataGet(I2C0_BASE);

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(I2CMasterBusy(I2C0_BASE));

    data2 = I2CMasterDataGet(I2C0_BASE);
    return ((uint16_t)data1 << 8 | data2);
}


void i2c_write(uint8_t slave_addr, uint8_t reg, int16_t data)
{
    int i;
    uint8_t buffer[3];
    uint32_t command[3] = {I2C_MASTER_CMD_BURST_SEND_START,
                           I2C_MASTER_CMD_BURST_SEND_CONT,
                           I2C_MASTER_CMD_BURST_SEND_FINISH};
    buffer[0] = reg;

    switch(reg)
    {
    case TEMP_REG:
        UARTprintf("Temperature register is read only register\n\r");
        break;

    case CONFIG_REG:
        buffer[1] = data >> 8;
        buffer[2] = data;
        break;

    case TLOW_REG:
    case THIGH_REG:
        data = data/0.0625;
        buffer[1] = data >> 4;
        buffer[2] = data << 4;
        break;
    }

    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
    for(i=0; i<3; i++)
    {
        I2CMasterDataPut(I2C0_BASE, buffer[i]);
        I2CMasterControl(I2C0_BASE, command[i]);
        while(I2CMasterBusy(I2C0_BASE));
    }
}


void configure_i2c_master()
{
    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));

    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}


void Timer2Callback(TimerHandle_t xtimer)
{
    measureTemperature = true;
}


float read_temperature(uint8_t slave_addr, uint8_t reg)
{
    int16_t temperature;
    float final_temp;

    if(reg == CONFIG_REG)
    {
        UARTprintf("Configuration register is not temperature\n\r");
    }
    else
    {
        temperature = i2c_read(slave_addr, reg);
        temperature = temperature >> 4;
        final_temp = temperature * 0.0625;
        return final_temp;
    }
    return -1000;
}

