/*
 * control.c
 *
 * Created: 6/17/2019 7:58:12 PM
 *  Author: horva
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "control.h"
#include "bitop.h"

//Display data
const uint8_t Digit_data[DIGIT_NUM] = {
	//   0          1          2          3          4
	0b11111100,0b01100000,0b11011010,0b11110010,0b01100110,
	//   5          6          7          8          9
	0b10110110,0b10111110,0b11100000,0b11111110,0b11110110,
	//              E          -
	0b00000000,0b10011110,0b00000010
};

//Track which digit to load into shift register
uint8_t digitCounter = 0;

ISR(TIMER0_OVF_vect){
	//Update screen
	sh_shiftDigit(digits[digitCounter]);
	sh_switchCathode(digitCounter++);
	if(digitCounter==5){digitCounter=0;}
	
	//Poll encoder
	enc_a = (PINB&0x2)>>1;
	enc_b = (PINB&0x4)>>2;
		
	if(!enc_a && !enc_b && enc_b_old){
		//RIGHT
		enc_rdy = 0;
		}else if(!enc_a && enc_b && !enc_b_old){
		//LEFT
		enc_rdy = 0;
	}
	
	enc_a_old = enc_a;
	enc_b_old = enc_b;
}

void control_init(){
	//Set default values
	enc_rdy = 1;
	enc_a = 1;
	enc_b = 1;
	enc_a_old = 1;
	enc_b_old = 1;
	
	//Set shift register pins as outputs
	setBit(SH_DDR,SH_DATA);
	setBit(SH_DDR,SH_SHIFT_CLOCK);
	setBit(SH_DDR,SH_STORAGE_CLOCK);
	setBit(SH_DDR,SH_OE);
	
	//Enable internal pull-up for encoder pins
	setBit(PORTB,0);
	setBit(PORTB,1);
	setBit(PORTB,2);
	
	//Setup Timer 0
	TCCR0B |= 0x3;
	TIMSK0 |= 0x1;	//Enable overflow interrupt
	
	//Enable global interrupts
	sei();
}

void sh_pulseSH(){
	setBit(SH_PORT,SH_SHIFT_CLOCK);
	clearBit(SH_PORT,SH_SHIFT_CLOCK);
}

void sh_pulseST(){
	setBit(SH_PORT,SH_STORAGE_CLOCK);
	clearBit(SH_PORT,SH_STORAGE_CLOCK);
}

void sh_shiftDigit(uint8_t d){
	for(uint8_t i=0;i<8;i++){
		changeBit(SH_PORT,SH_DATA,(Digit_data[d]&(1<<i))>>i);
		sh_pulseSH();
	}
	sh_pulseST();
}

void sh_switchCathode(uint8_t c){
	//Reset cathodes
	CAT_DDR &= ~(0b11111000);
	
	//Set active cathode
	CAT_DDR |= (1<<(c+3));
}
