/*
 * RTC.c
 *
 * Created: 6/19/2019 11:08:24 PM
 *  Author: Dave
 */
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <stdint.h>
#include <util/twi.h>

#include "control.h"
#include "RTC.h"

void RTC_init(void) {
    //Set I2C clock frequency divider
    TWBR = 6;
}

uint8_t RTC_getData(uint8_t num) {
    //Send start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    //Wait for start condition to be sent
    while (!(TWCR & (1 << TWINT)));
    //Check if start was sent
    if ((TWSR & 0xF8) != TW_START) {
        return 1;
    }

    //Send slave address in write mode
    TWDR = SLA_W;
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for reply
    while (!(TWCR & (1 << TWINT)));
    //Check for ACK
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK) {
        return 2;
    }

    //Load data address
    TWDR = 0x00;
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for reply
    while (!(TWCR & (1 << TWINT)));
    //Check for ACK
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
        return 3;
    }

    //Send repeated start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    //Wait for start condition to be sent
    while (!(TWCR & (1 << TWINT)));
    //Check if repeated start was successful
    if ((TWSR & 0xF8) != TW_REP_START) {
        return 4;
    }

    //Send slave address in read mode
    TWDR = SLA_R;
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for reply
    while (!(TWCR & (1 << TWINT)));
    //Check for ACK
    if ((TWSR & 0xF8) != TW_MR_SLA_ACK) {
        return 5;
    }

    for (uint8_t i = 0; i < num; i++) {
        if (i == num - 1) {
            //Receive byte with NACK (end)
            TWCR = (1 << TWINT | 1 << TWEN);
            //Wait for reply
            while (!(TWCR & (1 << TWINT)));
            //Check for ACK
            if ((TWSR & 0xF8) != TW_MR_DATA_NACK) {
                return 6;
            }
        } else {
            //Receive byte with ACK (continue)
            TWCR = (1 << TWINT | 1 << TWEN | 1 << TWEA);
            //Wait for reply
            while (!(TWCR & (1 << TWINT)));
            //Check for ACK
            if ((TWSR & 0xF8) != TW_MR_DATA_ACK) {
                return 7;
            }
        }
        RTC_Data[i] = TWDR;
    }

    //Send stop
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    return 0;
}

uint8_t RTC_setData(void) {
    //Send start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    //Wait for start condition to be sent
    while (!(TWCR & (1 << TWINT)));
    //Check if start was sent
    if ((TWSR & 0xF8) != TW_START) {
        return 1;
    }

    //Send slave address in write mode
    TWDR = SLA_W;
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for reply
    while (!(TWCR & (1 << TWINT)));
    //Check for ACK
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK) {
        return 2;
    }

    //Load data address
    TWDR = 0x00;
    TWCR = (1 << TWINT) | (1 << TWEN);
    //Wait for reply
    while (!(TWCR & (1 << TWINT)));
    //Check for ACK
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
        return 3;
    }

    for (uint8_t i = 0; i < 7; i++) {
        //Load data
        TWDR = RTC_Data[i];
        TWCR = (1 << TWINT) | (1 << TWEN);
        //Wait for reply
        while (!(TWCR & (1 << TWINT)));
        //Check for ACK
        if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
            return 4;
        }
    }

    //Send stop
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    return 0;
}

void RTC_error(uint8_t n) {
    digits[0] = D_EMPTY;
    digits[1] = D_EMPTY;
    digits[2] = D_EMPTY;
    digits[3] = D_ERROR;
    digits[4] = n;
}
