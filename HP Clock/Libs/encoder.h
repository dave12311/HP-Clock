/*
 * encoder.h
 *
 * Created: 6/19/2019 12:46:33 PM
 *  Author: horva
 */ 
#include <stdint.h>

#ifndef ENCODER_H_
#define ENCODER_H_

#define ENC_DDR DDRB
#define ENC_PORT PORTB
#define ENC_PIN PINB

#define ENC_BUTTON 14
#define ENC_A 15
#define ENC_B 16

void enc_init();



#endif /* ENCODER_H_ */