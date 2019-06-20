/*
 * control.h
 *
 * Created: 6/17/2019 7:55:43 PM
 *  Author: horva
 */ 

#include <stdint.h>

#include "bitop.h"

#ifndef CONTROL_H_
#define CONTROL_H_

//Shift register definitions
#define SH_DDR DDRC
#define SH_PORT PORTC

#define SH_DATA 0
#define SH_SHIFT_CLOCK 1
#define SH_STORAGE_CLOCK 2
#define SH_OE 3

//Cathode definitions
#define CAT_DDR DDRD

//Number of displayable characters
#define DIGIT_NUM 13

//Special character definitions
#define D_EMPTY 10
#define D_ERROR 11
#define D_DASH 12

//Turn on or off the shift register
#define SH_ON() (clearBit(SH_PORT,SH_OE))
#define SH_OFF() (setBit(SH_PORT,SH_OE))

//Display data
const uint8_t Digit_data[DIGIT_NUM];

//Buffer for all 5 displayed digits
uint8_t digits[5];

//Track which digit to load into shift register
uint8_t digitCounter;

uint8_t enc_rdy;
uint8_t enc_a, enc_b;
uint8_t enc_a_old, enc_b_old;

void control_init();
void sh_pulseSH();
void sh_pulseST();
void sh_switchCathode(uint8_t c);

void sh_shiftDigit(uint8_t d);

#endif /* CONTROL_H_ */