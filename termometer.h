/*
 * Termometer.h
 *
 *  Created on: 18 de mai de 2022
 *      Author: Daniel Valentin
 */

#ifndef TERMOMETER_H_
#define TERMOMETER_H_

/* CONSTANTS */
#define TMP006_SLAVE_ADDRESS    0x40

/* TEMPERATURE SENSOR REGISTER DEFINITIONS */
#define TMP006_P_VOBJ           0x00
#define TMP006_P_TABT           0x01
#define TMP006_WRITE_REG        0x02
#define TMP006_P_MAN_ID         0xFE
#define TMP006_P_DEVICE_ID      0xFF

/* CONFIGURATION REGISTER SETTINGS */
#define TMP006_RST              0x8000
#define TMP006_POWER_DOWN       0x0000
#define TMP006_POWER_UP         0x7000
#define TMP006_CR_4             0x0000
#define TMP006_CR_2             0x0200
#define TMP006_CR_1             0x0400
#define TMP006_CR_0_5           0x0600
#define TMP006_CR_0_25          0x0800
#define TMP006_EN               0x0100
#define TMP006_DRDY             0x0080

void termometerStart(void);

long double termometerCurrentValue(void);

int I2C_read16(unsigned char);
void I2C_write16(unsigned char pointer, unsigned int writeByte);
void I2C_setslave(unsigned int slaveAdr);

#endif /* TERMOMETER_H_ */
