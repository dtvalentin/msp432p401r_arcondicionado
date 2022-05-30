/*
 * Display.cpp
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#include <grlib.h>
#include "Crystalfontz128x128_ST7735.h"
#include "display.h"

/* Graphic library context */
Graphics_Context g_sContext;

void displayStart(void)
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    displayClear(NORMAL);
}

void setBackGround(DisplayBackground db)
{
    switch (db)
    {
    case NORMAL:
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        break;
    case INVERTED:
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        break;
    }

}
void displayClear(DisplayBackground db)
{
    setBackGround(db);
    Graphics_clearDisplay(&g_sContext);
}

void displayPrintText(int x, int y, int8_t * text)
{
    Graphics_drawString(&g_sContext, text, AUTO_STRING_LENGTH, x, y,
    OPAQUE_TEXT);
}

