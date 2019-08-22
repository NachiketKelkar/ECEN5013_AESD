/*
 * Temperature.h
 *
 *  Created on: Apr 6, 2019
 *      Author: nachi
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

/*
 * Name: TemperatureTask()
 * Description: This the function of the temperature task which measures the temperature
 *              continuously and reports it to logger.
 * @param: parameters
 * @return: void
 */
void TemperatureTask(void *pvParameters);


/*
 * Name: Timer2Callback()
 * Description: This function is the callback function for the timer which is set to 1 second.
 *              It tells the temperature task to take the temperature readings and send to log
 * @param: void
 * @return: void
 */
void Timer2Callback();


/*
 * Name: i2c_init()
 * Description: This function initialises the i2c module for temperature sensor
 * @param: void
 * @return: void
 */
void i2c_init();


/*
 * Name: configure_i2c_master()
 * Description: This function configures the i2c for communication.
 * @param: void
 * @return: void
 */
void configure_i2c_master();


/*
 * Name: i2c_read()
 * Description: This function is used to read the data from the sensor registers.
 * @param: uint8_t (slave address), uint8_t (register to read data from)
 * @return: uint16_t (contents of the register)
 */
uint16_t i2c_read(uint8_t, uint8_t);


/*
 * Name: i2c_write()
 * Description: This function is used to write the data to the temperature sensor register
 * @param: uint8_t (slave address), uint8_t (register to write to), uint16_t (data to write)
 * @return: void
 */
void i2c_write(uint8_t, uint8_t, int16_t);


/*
 * Name: read_temperature()
 * Description: This function reads the register converts it to the actual temperature value
 *              and returns it back.
 * @param: uint8_t (slave address), uint8_t (register address to read the temperature from)
 * @return: float (temperature values in Celsius)
 */
float read_temperature(uint8_t, uint8_t);

#define I2C_INIT i2c_init();
#define CONFIGURE_MASTER configure_i2c_master();
#define TEMP_SENS_ADDR 0x48
#define TEMP_REG       0x00
#define CONFIG_REG     0x01
#define TLOW_REG       0x02
#define THIGH_REG      0x03

#endif /* TEMPERATURE_H_ */
