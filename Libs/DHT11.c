/*
 * DHT11.h
 *
 * Created: 6/25/2019 10:33:43 AM
 *  Author: Dave
 */
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "bitop.h"
#include "DHT11.h"
#include "control.h"

uint8_t DHT_start(void) {
    cli();
    _delay_ms(1500);
    clearBit(PORTD,2);
    setBit(DDRD,2);
    _delay_ms(25);
    clearBit(DDRD,2);
    setBit(PORTD,2);
    while (PIND & (1 << 2));
    while ((PIND & (1 << 2)) == 0);
    while (PIND & (1 << 2));
    sei();
    return 0;
}

uint8_t DHT_readByte(void) {
    for (uint8_t i = 0; i < 8; i++) {
        while ((PIND & (1 << 2)) == 0);
        _delay_us(30);
        if (PIND & (1 << 2))
            DHT_byte = (DHT_byte << 1) | (0x01);
        else
            DHT_byte = (DHT_byte << 1);
        while (PIND & (1 << 2));
    }
    return DHT_byte;
}

void DHT_readData(void) {
    DHT_hum_h = DHT_readByte();
    DHT_hum_l = DHT_readByte();
    DHT_temp_h = DHT_readByte();
    DHT_temp_l = DHT_readByte();
}
