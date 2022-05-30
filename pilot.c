/*
 * Pilot.cpp
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#include "msp.h"
#include "pilot.h"

void pilotStart()
{
    P2->DIR |= BIT2; // Led Blue
    P2->OUT &= ~BIT2;

    P2->DIR |= BIT1; // Led Green
    P2->OUT &= ~BIT1;

}

void pilotBlue(PilotState state)
{
    if (state == ON)
    {
        P2->OUT |= BIT2;
    }
    else
    {
        P2->OUT &= ~BIT2;
    }
}


void pilotGreen(PilotState state)
{
    if (state == ON)
    {
        P2->OUT |= BIT1;
    }
    else
    {
        P2->OUT &= ~BIT1;
    }
}
