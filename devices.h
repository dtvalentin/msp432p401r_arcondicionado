/*
 * devices.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef DEVICES_H__
#define DEVICES_H__

#include "msp.h"
#include "driverlib.h"

#define PORT_BASE  0x40004c00
#define RTC1_BASE   0x40004400

#define P1SEL0_ADDR 0x40004c0A
#define P1SEL1_ADDR 0x40004c0C

#define USCI_A0BASE  0x40001000

#define NVIC_BASE_ADDRESS 0xE000E000

#endif /* DEVICES_H__ */
