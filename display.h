/*
 * Display.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

typedef enum
{
    NORMAL = 0, INVERTED = 1
} DisplayBackground;

void displayStart(void);

void displayPrintText(int x, int y, int8_t * text);

void displayClear(DisplayBackground db);

#endif /* DISPLAY_H_ */
