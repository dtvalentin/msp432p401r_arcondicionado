/*
 * Clock.cpp
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#include "clock.h"

#include <stdio.h>
#include <string.h>

// Registradores do RTC
volatile unsigned short int * pRTCCTL0;

volatile unsigned short int * pRTCCTL13;

volatile unsigned short int * pRTCTIM0;

volatile unsigned short int * pRTCTIM1;

volatile unsigned short int * pRTCDATE;

volatile unsigned short int * pRTCYEAR;

void clockDateTimeToString(DateTime dt, char* buffer)
{
    // Converte em String
    sprintf(buffer, "%02d:%02d:%02d %02d/%02d/%04d", dt.hour, dt.minute,
            dt.second, dt.day, dt.month, dt.year);
}

void clockStart(void)
{
    pRTCCTL0 = (volatile unsigned short int *) (RTC1_BASE + 0x0);
    pRTCCTL13 = (volatile unsigned short int *) (RTC1_BASE + 0x2);
    pRTCTIM0 = (volatile unsigned short int *) (RTC1_BASE + 0x10);
    pRTCTIM1 = (volatile unsigned short int *) (RTC1_BASE + 0x12);
    pRTCDATE = (volatile unsigned short int *) (RTC1_BASE + 0x14);
    pRTCYEAR = (volatile unsigned short int *) (RTC1_BASE + 0x16);

    // Configuração do RTC
    *pRTCCTL0 = (0xA500); //desbloqueia o RTC, 0 em todos os bits de configuração de interrupção

    DateTime dt;
    dt.year = 2001;
    dt.month = 01;
    dt.day = 01;
    dt.hour = 0;
    dt.minute = 0;
    dt.second = 0;

    clockAdjust(dt);

}

void clockAdjust(DateTime dt)
{

    *pRTCCTL13 = (0 << 7) +  //hexa
            (1 << 6); //hold for a while

    // Inicia RTC com hh:mm:ss do dia DD/MM/YYYY
    *pRTCTIM0 = (dt.minute << 8) + (dt.second << 0);
    *pRTCTIM1 = (0 << 8) + (dt.hour << 0);
    *pRTCDATE = (dt.month << 8) + (dt.day << 0);
    *pRTCYEAR = dt.year;

    *pRTCCTL13 = 0;  //releases hold

}

DateTime clockCurrentDateTime()
{
    // Aguarda estar ready
    while ((*pRTCCTL13 & (1 << 4)) == 0)
    {
        // Aguarda
    }

    DateTime dt;
    dt.hour = (*pRTCTIM1 & 0xFF);
    dt.minute = (*pRTCTIM0 >> 8);
    dt.second = (*pRTCTIM0 & 0x3F);

    dt.day = (*pRTCDATE & 0xFF);
    dt.month = (*pRTCDATE >> 8);
    dt.year = (*pRTCYEAR);

    return dt;
}

void clockWaitSecond(void)
{
    while ((*pRTCCTL13 & (1 << 4)) != 0)
    {
        // Espera o próximo segundo (verificando o rdy, que só vai baixar quando
        // Estiver próximo da borda de segundo)
    }

}
