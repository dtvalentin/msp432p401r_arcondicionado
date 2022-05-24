/*
 * Pilot.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef PILOT_H_
#define PILOT_H_

typedef enum
{
    OFF = 0, ON = 1
} PilotState;

void pilotStart();

void pilotBlue(PilotState state);

void pilotGreen(PilotState state);

#endif /* PILOT_H_ */
