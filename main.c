//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "devices.h"
#include "clock.h"
#include "console.h"
#include "display.h"
#include "pilot.h"
#include "luximeter.h"
#include "termometer.h"
#include "datalogger.h"

char cdt[50];
char bufferOut[50];
char bufferIn[50];

int temp = 20;
int max = 25;
bool dark = false;
bool ligthUpdate = true;
bool tempUpdate = true;
bool updateClock = true;
bool powerOnOff = false;
bool printPowerState = true;
bool changeMax = false;
int timeInterval = 10;
int lastcmd = 0;

const char * LUZ_APAGADA = "Lux Apagada";
const char * LUZ_ACESA = "Luz Acesa...";
const char * AR_LIGADO = "Ar Ligado...";
const char * AR_DESLIGADO = "Ar Desligado...";
const char * AR_COND_LIGADO = "Ar-condicionado ligado";
const char * AR_COND_DESLIGADO = "Ar-condicionado desligado";

void showHelp(void);
void comandProcessor(const char* data);
bool updateDateTime(const char * data);

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer

    consoleStart(19200);

    consolePrintln("\n\rIniciando Aplicativo...");

    consolePrintln("\n\rIniciando Dispositivos...");
    clockStart();
    consolePrintln("Relogio Inicializado...");

    displayStart();
    consolePrintln("Display Inicializado...");

    pilotStart();
    consolePrintln("Luz Piloto Inicializada...");

    luximeterStart();
    consolePrintln("Luximetro Inicializado...");

    termometerStart();
    consolePrintln("Termometro Inicializado...");

    dataLoggerStart();
    consolePrintln("Datalogger Inicializado...");

    showHelp();

    // Configura a GPIO
    //P1->DIR |= BIT0; // Led 1
    //P1->OUT &= ~BIT0;

    // Seta Timer 1 Para Modo Periodico em 32 bits, Sem pre-scaler
    TIMER32_1->CONTROL = TIMER32_CONTROL_SIZE | TIMER32_CONTROL_MODE;

    // Valor Inicial de Contagem
    TIMER32_1->LOAD = 0x1000000;

    // Habilita Interrupção no NVIC
    int x = ((T32_INT1_IRQn) & 31);
    NVIC->ISER[0] = 1 << x;

    // Habilita contagem do timer e interrupção
    TIMER32_1->CONTROL |= TIMER32_CONTROL_ENABLE | TIMER32_CONTROL_IE;

    while (1)
    {
        // Leitura de Sensores a cada 10 Segundos
        if (timeInterval >= 10)
        {
            // Coleta e Analise de Luz
            if (luximeterDetectDarkness())
            {
                if (!dark)
                    ligthUpdate = true;
                dark = true;
            }
            else
            {
                if (dark)
                    ligthUpdate = true;
                dark = false;
            }

            // Coleta e Analise de Temperatura
            float tmpf = termometerCurrentValue();
            float tmpc = (tmpf - 32.0) * (5.0 / 9.0);
            int newTemp = (int) (tmpc);
            if (newTemp != temp || changeMax)
            {
                temp = newTemp;
                if (temp > max)
                    powerOnOff = true;
                else
                    powerOnOff = false;
                tempUpdate = true;
            }

            timeInterval = 0;
        }

        // Atualização de mudança de estado da iluminação
        if (ligthUpdate)
        {
            if (dark)
            {
                displayClear(NORMAL);
                displayPrintText(2, 10, LUZ_APAGADA);
                consolePrintln(LUZ_APAGADA);
                dataLoggerRegisterEvent(cdt, EVENTO_LUZ_APAGADA);
            }
            else
            {
                displayClear(INVERTED);
                displayPrintText(2, 10, LUZ_ACESA);
                consolePrintln(LUZ_ACESA);
                dataLoggerRegisterEvent(cdt, EVENTO_LUZ_LIGADA);
            }
            pilotGreen(ON);
            ligthUpdate = false;
            tempUpdate = true;
        }

        // Atualização de mudança de estado da temperatura
        if (tempUpdate)
        {
            if (powerOnOff)
            {
                displayPrintText(2, 30, AR_LIGADO);
                if (powerOnOff != printPowerState)
                {
                    consolePrintln(AR_COND_LIGADO);
                    pilotBlue(ON);
                    dataLoggerRegisterEvent(cdt, EVENTO_AR_LIGADO);
                    printPowerState = true;
                }
            }
            else
            {
                displayPrintText(2, 30, AR_DESLIGADO);
                if (powerOnOff != printPowerState)
                {
                    consolePrintln(AR_COND_DESLIGADO);
                    pilotBlue(OFF);
                    dataLoggerRegisterEvent(cdt, EVENTO_AR_DESLIGADO);
                    printPowerState = false;
                }
            }

            sprintf(bufferOut, "Temp. Ambiente: %.2dC ", temp);
            displayPrintText(2, 50, bufferOut);

            sprintf(bufferOut, "Temp. Maxima: %.2dC", max);
            displayPrintText(2, 70, bufferOut);

            tempUpdate = false;
            changeMax = false;
        }

        // Desliga Verde em 5 Segundos
        if (timeInterval == 5)
        {
            pilotGreen(OFF);
        }

        if (updateClock)
        {
            sprintf(bufferOut, "Timer: %0.2ds", timeInterval);
            displayPrintText(2, 90, bufferOut);

            //clockWaitSecond();
            DateTime dt = clockCurrentDateTime();
            clockDateTimeToString(dt, cdt);
            displayPrintText(6, 110, cdt);
            updateClock = false;
        }

        if (consoleReadln(bufferIn))
        {
            comandProcessor(bufferIn);
        }
    }
}

bool updateMaxTemperature(const char * data)
{
    int tp;
    if (sscanf(data, "%u", &tp) == 1)
    {
        max = tp;
        changeMax = true;
        return true;
    }
    return false;
}

bool updateDateTime(const char * data)
{
    int dd, mm, yy;
    int hrs, min, sec;

    if (sscanf(data, "%u:%u:%u %u/%u/%u", &hrs, &min, &sec, &dd, &mm, &yy) == 6)
    {
        DateTime dt;
        dt.year = yy;
        dt.month = mm;
        dt.day = dd;
        dt.hour = hrs;
        dt.minute = min;
        dt.second = sec;
        clockAdjust(dt);
        return true;
    }

    return false;
}

void showHelp()
{
    consolePrintln("-----------------------------------------------------");
    consolePrintln("1 ---> Para exibir comandos");
    consolePrintln("2 ---> Ajustar o relogio");
    consolePrintln("3 ---> Ajustar a temperatura maxima");
    consolePrintln("4 ---> Mostrar o registro");
    consolePrintln("-----------------------------------------------------");
}

void comandProcessor(const char* data)
{
    int cmd = atoi(data);
    if (cmd == 1)
    {
        showHelp();
        lastcmd = 1;
    }
    else if (cmd == 2)
    {
        consolePrintln("Ajuste de Data e Hora");
        consolePrintln("---------------------------------------------------");
        consolePrintln("Digite a data e hora hh:mm:ss DD/MM/YYY ");
        lastcmd = 2;
    }
    else if (cmd == 3)
    {
        char * buffer[60];
        sprintf(buffer, "Ajuste da temperatura maxima (Atual: %0.2d C)", max);
        consolePrintln(buffer);
        consolePrintln("---------------------------------------------------");
        consolePrintln("Digite um valor entre 0 e 99");
        lastcmd = 3;
    }
    else if (cmd == 4)
    {
        consolePrintln("Relatorio de eventos");
        consolePrintln("---------------------------------------------------");
        lastcmd = 4;
    }
    else if (lastcmd == 2)
    {
        if (updateDateTime(data) == 1)
            consolePrintln("Data e Hora atualizada");
        else
            consolePrintln("Erro na atualizacao da Data e Hora");

        lastcmd = 0;
    }
    else if (lastcmd == 3)
    {
        if (updateMaxTemperature(data) == 1)
        {
            consolePrintln("Temperatura atualizada");
            tempUpdate = true;
        }
        else
            consolePrintln("Erro na atualizacao da temperatura");

        lastcmd = 0;
    }
    else
    {
        consolePrintln("Comando invalido");
        lastcmd = 0;
    }
}

// Serviço de Atendimento de Interrupção
void T32_INT1_IRQHandler(void)
{
    TIMER32_1->INTCLR |= BIT0;              // Limpa o Timer
    //P1->OUT ^= BIT0;                        // Pisca LED
    timeInterval++;
    updateClock = true;
}
