/*
 * Clock.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "devices.h"

typedef struct
{
    unsigned short int year;
    unsigned short int month;
    unsigned short int day;
    unsigned short int hour;
    unsigned short int minute;
    unsigned short int second;
} DateTime;

void clockStart(void);

void clockDateTimeToString(DateTime dt, char* buffer);

void clockAdjust(DateTime dt);

DateTime clockCurrentDateTime(void);

void clockWaitSecond(void);

#endif /* CLOCK_H_ */
