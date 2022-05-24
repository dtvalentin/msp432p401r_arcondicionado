/*
 * Pilot.cpp
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#include "msp.h"
#include "driverlib.h"
#include "luximeter.h"

void luximeterStart()
{

    //inicialização da I2C -> canal de comunicação para o light sensor

    //habilita função primária
    //pinos P6.4 e P6.5 -> via header para os sinais SDA e SCL do BoosterPack
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN4,
    GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN5,
    GPIO_PRIMARY_MODULE_FUNCTION);

    //inicialização da I2C -> considera SMCLK de 12 MHz
    eUSCI_I2C_MasterConfig i2c_config;
    i2c_config.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    i2c_config.i2cClk = 12000000;
    i2c_config.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    i2c_config.byteCounterThreshold = 0;
    i2c_config.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;

    I2C_disableModule(EUSCI_B1_BASE);
    I2C_initMaster(EUSCI_B1_BASE, &i2c_config);

    I2C_enableModule(EUSCI_B1_BASE);
    //inicializa sensor OPT3001
    //registrador CONFIGURATION -> endereço 0x01

    //0. ajustes no master para envio

    I2C_setSlaveAddress(EUSCI_B1_BASE, SENSOR_I2C_ADDRESS);
    //ajusta modo I2C para escrita
    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    //1. monta conteúdo do registrador
    uint16_t content = (0xC << 12) + // auto full-scale mode
            (0 << 11) +         //100 ms
            (0x2 << 9);         //continuous conversion
//          (0 << 9);           //shutdown mode

    //2. envia endereço do registrador
    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, CONFIG_REG_ADDR);

    //3. envia conteúdo do registrador -> se for último, usa API de fim
    I2C_masterSendMultiByteNext(EUSCI_B1_BASE, (content >> 8) & 0xFF);  //MSB
    I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, (content & 0xFF));
}

uint32_t luximeterRead()
{
    uint16_t raw_value;
    uint16_t mantissa;
    uint8_t exponent;
    uint32_t value;
    uint32_t factor = 1;

    I2C_setSlaveAddress(EUSCI_B1_BASE, SENSOR_I2C_ADDRESS);

    //ajusta modo I2C para escrita -> enviar o endereço do registrador
    I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Limpa flag de interrupções */
    I2C_clearInterruptFlag(EUSCI_B1_BASE,
    EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Espera estar pronto */
    while (I2C_isBusBusy(EUSCI_B1_BASE))
        ;

    //envia endereço do registrador
    I2C_masterSendSingleByte(EUSCI_B1_BASE, RESULT_REG_ADDR);
    //verifica se irq de condição de parada foi gerada
    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_STOP_INTERRUPT)))
        ;

    //muda para leitura e recebe o resultado
    I2C_masterReceiveStart(EUSCI_B1_BASE);

    //verifica se irq de condição de Rx foi gerada
    while (!(I2C_getInterruptStatus(EUSCI_B1_BASE,
    EUSCI_B_I2C_RECEIVE_INTERRUPT0)))
        ;

    //recebe o MSB
    raw_value = (I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE) << 8);
    //recebe o LSB e encerra
    raw_value |= I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);

    //processa o resultado para gerar os lux
    exponent = (uint8_t) (raw_value >> 12);
    mantissa = raw_value & 0xFFF;
    while (exponent > 0)
    {
        factor = factor * 2;
        exponent--;
    }

    value = mantissa * factor;
    return value;

}

bool luximeterDetectDarkness()
{
    uint32_t value = luximeterRead();
    if (((double) value / 100) > 50)
    {
        return false;
    }
    else
    {
        return true;
    }
}

