/*
 * Pilot.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef LUXIMETER_H_
#define LUXIMETER_H_

//Endereço do sensor no bus I2C
//Como definir? Pág. 12, seção 7.3.4.1 datasheet do sensor
// Pino Addr em GND -> endereço 0b1000100 -> 0x44
#define SENSOR_I2C_ADDRESS 0x44
#define CONFIG_REG_ADDR    0x01
#define RESULT_REG_ADDR    0x00

void luximeterStart();

uint32_t luximeterRead();

bool luximeterDetectDarkness();

#endif /* LUXIMETER_H_ */
