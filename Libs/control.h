/*
 * control.h
 *
 * Created: 6/17/2019 7:55:43 PM
 *  Author: Dave
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

#define BLINK_MODE_ALL      0
#define BLINK_MODE_RIGHT    1
#define BLINK_MODE_LEFT     2
#define BLINK_MODE_MIDDLE   3

//Display data
const uint8_t Digit_data[DIGIT_NUM];

//Buffer for all 5 displayed digits
volatile uint8_t digits[5];

//Track which digit to load into shift register
volatile uint8_t digitCounter;

//Stores blink state
volatile uint8_t blink;
volatile uint8_t blinkMode;

//Store encoder states
volatile uint8_t enc_a, enc_b;
volatile uint8_t enc_a_old, enc_b_old;
volatile uint8_t enc_button, enc_button_old;
volatile uint8_t encButtonPressed;

//Encoder event pointers
void (*volatile encoderEventRight)(void);

void (*volatile encoderEventLeft)(void);

//Encoder events
void EventYearRight(void);

void EventYearLeft(void);


void EventMonthRight(void);

void EventMonthLeft(void);


void EventDateRight(void);

void EventDateLeft(void);


void EventHourRight(void);

void EventHourLeft(void);


void EventMinRight(void);

void EventMinLeft(void);

//Start timers, set I/O
void control_init(void);

//Pulse the shift register clock
void sh_pulseSH(void);

//Pulse the storage register clock
void sh_pulseST(void);

//Shift out one digit
void sh_shiftDigit(uint8_t d);

//Cycle to the next cathode
void switchCathode(uint8_t c);

//Start clock setup
void startSetup(void);


void displayTime(void);

void displayDate(void);

void blinkOn(void);

void blinkOff(void);

void resetBlink(void);

#endif /* CONTROL_H_ */