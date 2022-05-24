/*
 * Datalogger.cpp
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "msp.h"
#include "driverlib.h"
#include "datalogger.h"

#define LOGFILE_SIZE 5

char * logFile[LOGFILE_SIZE];

int logIndex = 0;
int line = 0;

void dataLoggerStart()
{
    FlashCtl_setWaitState(FLASH_BANK0, 0);
    FlashCtl_setWaitState(FLASH_BANK1, 0);
}

void dataLoggerRegisterEvent(char * dateTime, int eventType)
{
    char logline[25];
    sprintf(logline, "%c, %1d", dateTime, eventType);
    strcpy(logFile[logIndex++], logline);

    if (logIndex == LOGFILE_SIZE)
    {
        for (line = 0; line < LOGFILE_SIZE; line++)
        {
            FlashCtl_programMemory(logFile[line], (void*) 0x20000,
                                   strlen(logFile[line]));
        }
        logIndex = 0;
    }

}

void dataLoggerReadEvents()
{
    // Faltou a leitura da flash
}
