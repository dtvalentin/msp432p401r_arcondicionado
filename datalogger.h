/*
 * Datalogger.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#define EVENTO_AR_LIGADO 1
#define EVENTO_AR_DESLIGADO 2
#define EVENTO_LUZ_LIGADA 3
#define EVENTO_LUZ_APAGADA 4

void dataLoggerStart();

void dataLoggerRegisterEvent(char * dateTime, int eventType);

void dataLoggerReadEvents();

#endif /* DATALOGGER_H_ */
