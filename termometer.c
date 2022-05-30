/*
 * Termometer.cpp
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#include "termometer.h"
#include "msp.h"
#include "driverlib.h"
#include "math.h"

static long double S0 = 0;

void termometerStart(void)
{
    I2C_setslave(TMP006_SLAVE_ADDRESS);
    I2C_write16(TMP006_WRITE_REG, TMP006_RST);
    volatile int i;
    for (i = 10000; i > 0; i--)
    {
        // Espera um tempo
    }
    I2C_write16(TMP006_WRITE_REG, TMP006_POWER_UP | TMP006_CR_2);
}

long double termometerCurrentValue(void)
{
    volatile int Vobj = 0;
    volatile int Tdie = 0;

    I2C_setslave(TMP006_SLAVE_ADDRESS);

    Vobj = I2C_read16(TMP006_P_DEVICE_ID);

    I2C_setslave(TMP006_SLAVE_ADDRESS);

    Vobj = I2C_read16(TMP006_P_VOBJ);

    I2C_setslave(TMP006_SLAVE_ADDRESS);

    Tdie = I2C_read16(TMP006_P_TABT);
    Tdie = Tdie >> 2;

    long double Vobj2 = (double) Vobj * .00000015625;
    long double Tdie2 = (double) Tdie * .03525 + 273.15;

    S0 = 6 * pow(10, -14);
    long double a1 = 1.75 * pow(10, -3);
    long double a2 = -1.678 * pow(10, -5);
    long double b0 = -2.94 * pow(10, -5);
    long double b1 = -5.7 * pow(10, -7);
    long double b2 = 4.63 * pow(10, -9);
    long double c2 = 13.4;
    long double Tref = 298.15;

    long double S = S0
            * (1 + a1 * (Tdie2 - Tref) + a2 * pow((Tdie2 - Tref), 2));
    long double Vos = b0 + b1 * (Tdie2 - Tref) + b2 * pow((Tdie2 - Tref), 2);
    volatile long double fObj = (Vobj2 - Vos) + c2 * pow((Vobj2 - Vos), 2);
    volatile long double Tobj = pow(pow(Tdie2, 4) + (fObj / S), .25);
    Tobj = (9.0 / 5.0) * (Tobj - 273.15) + 32;

    return (Tobj);
}

/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig i2cConfig = {
EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        48000000,                               // SMCLK = 48MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
        };

int I2C_read16(unsigned char writeByte)
{
    int val = 0;
    int valScratch = 0;

    I2C_setMode(EUSCI_B1_BASE,
    EUSCI_B_I2C_TRANSMIT_MODE);

    I2C_clearInterruptFlag(EUSCI_B1_BASE,
    EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    while (I2C_isBusBusy(EUSCI_B1_BASE))
        ;

    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, writeByte);

    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE,
    EUSCI_B_I2C_TRANSMIT_INTERRUPT0)))
        ;

    I2C_masterSendMultiByteStop(EUSCI_B1_BASE);

    while (!I2C_getInterruptStatus(EUSCI_B1_BASE,
    EUSCI_B_I2C_STOP_INTERRUPT))
        ;
    I2C_masterReceiveStart(EUSCI_B1_BASE);

    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE,
    EUSCI_B_I2C_RECEIVE_INTERRUPT0)))
        ;

    val = I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);

    valScratch = I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);

    val = (val << 8);

    val |= valScratch;

    return (int16_t) val;
}

void I2C_write16(unsigned char pointer, unsigned int writeByte)
{
    I2C_setMode(EUSCI_B1_BASE,
    EUSCI_B_I2C_TRANSMIT_MODE);

    I2C_clearInterruptFlag(EUSCI_B1_BASE,
    EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    while (I2C_isBusBusy(EUSCI_B1_BASE))
        ;

    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, pointer);

    I2C_masterSendMultiByteNext(EUSCI_B1_BASE,
                                (unsigned char) (writeByte >> 8));

    I2C_masterSendMultiByteFinish(EUSCI_B1_BASE,
                                  (unsigned char) (writeByte & 0xFF));

}

void I2C_setslave(unsigned int slaveAdr)
{
    I2C_setSlaveAddress(EUSCI_B1_BASE, slaveAdr);

    I2C_clearInterruptFlag(EUSCI_B1_BASE,
    EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    return;
}
