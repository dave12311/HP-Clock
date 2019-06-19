/*
 * HP Clock.c
 *
 * Created: 6/17/2019 3:53:55 PM
 * Author : horva
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "Libs/bitop.h"
#include "Libs/shiftreg.h"

int main(void){
	for(uint8_t i=0;i<5;i++){
		digits[i] = 0;
	}
	
	sh_init();
	
    while (1){
	}
}

