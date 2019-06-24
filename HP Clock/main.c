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
#include "Libs/control.h"
#include "Libs/RTC.h"

int main(void){
	control_init();
	RTC_init();

	uint8_t err = 0;
	
	startSetup();
	RTC_setData();
	
    while (1){
		err = RTC_getData(2);
		if(err == 0){
			RTC_writeTime();
		}else{
			RTC_error(err);
		}
		_delay_ms(500);
	}
}

