/*
 * encoder.c
 *
 * Created: 6/19/2019 12:46:45 PM
 *  Author: horva
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "bitop.h"
#include "shiftreg.h"
#include "encoder.h"

ISR(PCINT1_vect){
	
}

void enc_init(){
	//Enable internal pull-up
	setBit(ENC_PORT,ENC_BUTTON);
	setBit(ENC_PORT,ENC_A);
	setBit(ENC_PORT,ENC_B);
	
	//Setup interrupts
	setBit(PCICR,PCIE1);
	setBit(PCMSK1,3);
	setBit(PCMSK1,4);
	setBit(PCMSK1,5);
}