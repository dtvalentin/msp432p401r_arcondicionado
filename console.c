/*
 * Console.cpp
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#include "msp.h"
#include "stdint.h"
#include "string.h"
#include "driverlib.h"
#include "devices.h"
#include "console.h"

int i;

unsigned int P1SEL0_OFF = 0x0A;
unsigned int P1SEL1_OFF = 0x0C;
unsigned int P1DIR_OFF = 0x04;

volatile unsigned short int * UCA0_CTLW0;
volatile unsigned short int * UCA0_BRW;
volatile unsigned short int * UCA0_MCTLW;
volatile unsigned short int * UCA0_TXBUF;
volatile unsigned short int * UCA0_RXBUF;
volatile unsigned short int * UCA0_IE;
volatile unsigned short int * UCA0_IFG;

//registradores do NVIC
volatile uint32_t* NVIC_ISER0 = (uint32_t*) (NVIC_BASE_ADDRESS + 0x100);

//caracter recebido
char g_carac;

//flag para indicar que recebeu algo
int g_rx_ok = 0;

void consoleStart(uint32_t rate)
{
    UCA0_CTLW0 = (volatile unsigned short int *) (USCI_A0BASE);
    UCA0_BRW = (volatile unsigned short int *) (USCI_A0BASE + 0x06);
    UCA0_MCTLW = (volatile unsigned short int *) (USCI_A0BASE + 0x08);
    UCA0_TXBUF = (volatile unsigned short int *) (USCI_A0BASE + 0x0E);
    UCA0_RXBUF = (volatile unsigned short int *) (USCI_A0BASE + 0x0C);
    UCA0_IE = (volatile unsigned short int *) (USCI_A0BASE + 0x1A);
    UCA0_IFG = (volatile unsigned short int *) (USCI_A0BASE + 0x1C);

    NVIC_ISER0 = (uint32_t*) (NVIC_BASE_ADDRESS + 0x100);

    //Seleciona P1.2 e P1.3 como função primária -> RX e TX UART
    *((volatile unsigned char*) (PORT_BASE + P1DIR_OFF)) &= ~0x0E;

    *((volatile unsigned char*) (PORT_BASE + P1SEL0_OFF)) |= 0x0E;

    *((volatile unsigned char*) (PORT_BASE + P1SEL1_OFF)) &= ~0x0E;

    //configura a fonte do clock SMCLK para 12MHz do clock externo
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    //reseta a UART0
    *UCA0_CTLW0 |= 0x1;

    //configura para 8-N-1, fonte de clock SMCLK
    *UCA0_CTLW0 |= (0x80);

    //configura baud rate (seção 22.3.13 do manual)
    //para 19200 bps -> UCOS16=1 UCBR0=78 UCBRF0=2 UCBRS0=0

    *UCA0_MCTLW |= (0x0 << 8) +  //UCBRS = 0
            (0x1 << 4) + //UCBRF=1
            0x1; //UCOS16 = 1
    *UCA0_BRW = 39;

    //remove reset
    *UCA0_CTLW0 &= ~0x1;

    //configura o NVIC
    //Cuidado! Ao escrever o valor diretamente, corre o risco de alterar
    //a configuração das outras interrupções! O ideal é fazer um OR.
    *NVIC_ISER0 = (0x1 << 16);

    //liberar o reset
    *UCA0_CTLW0 = *UCA0_CTLW0 & ~(0x1);

    //configurar a IRQ no ctrl de UART
    *UCA0_IE = 0x1;      //Rx interrupt enable

}

int consolePrintln(char * data)
{
    bool hasText = false;
    //envia caracter a caracter
    for (i = 0; i < strlen(data); i++)
    {
        *UCA0_TXBUF = data[i];

        while ((*UCA0_IFG & 0x2) == 0x0)
        {
            // Espera fim da transmissão
        }
        hasText = true;
    }

    if (hasText)
    {
        *UCA0_TXBUF = '\n';
        while ((*UCA0_IFG & 0x2) == 0x0)
        {
            // Espera fim da transmissão
        }
        *UCA0_TXBUF = '\r';
        while ((*UCA0_IFG & 0x2) == 0x0)
        {
            // Espera fim da transmissão
        }

    }
    return hasText;
}

bool consoleReadln(char * buffer)
{
    //se chegou caracter, envio de volta
    if (g_rx_ok == 1)
    {
        int i = 0;
        while (g_carac != 0x0)
        {
            buffer[i++] = g_carac;

            g_rx_ok = 0;

            if (g_carac == '\r')
            {
                g_rx_ok = 0;
                buffer[i++] = 0;
                return true;
            }

            while (g_rx_ok != 1)
            {
                // Espera pelo proximo
            }

        }
    }
    else
    {
        return false;
    }
    return true;
}

// Esta é a função que será chamada quando a IRQ de Rx ocorrer
void EUSCIA0_IRQHandler()
{
    //certamente chegou um caracter, então vamos ler
    g_carac = *UCA0_RXBUF;
    g_rx_ok = 1;

}
