/*
 * DHT11.h
 *
 * Created: 6/25/2019 10:33:43 AM
 *  Author: Dave
 */
#ifndef DHT11_H_
#define DHT11_H_

#include <avr/io.h>
#include <stdint.h>

#define DHT_setOut setBit(DDRD,2)
#define DHT_setIn clearBit(DDRD,2)
#define DHT_state ((PIND & (1<<2))>>2)

uint8_t ready, DHT_byte;
uint8_t DHT_temp_h, DHT_temp_l, DHT_hum_h, DHT_hum_l;

uint8_t DHT_start(void);

uint8_t DHT_readByte(void);

void DHT_readData(void);

#endif //DHT11_H_
