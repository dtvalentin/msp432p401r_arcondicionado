/*
 * Console.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

void consoleStart(uint32_t rate);

int consolePrintln(char * data);

bool consoleReadln(char * buffer);

#endif /* CONSOLE_H_ */
