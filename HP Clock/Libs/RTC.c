/*
 * RTC.c
 *
 * Created: 6/19/2019 11:08:24 PM
 *  Author: horva
 */ 
#include <util/twi.h>

#include "RTC.h"
#include "control.h"

void RTC_init(){
	//Set I2C clock frequency to 400kHz
	TWBR = 2;
}

uint8_t RTC_getData(uint8_t num){
	//Send start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	//Wait for start condition to be sent
	while (!(TWCR & (1<<TWINT)));
	//Check if start was sent
	if ((TWSR & 0xF8) != TW_START){
		return 1;
	}
	
	//Send slave address in write mode
	TWDR = SLA_W;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Wait for reply
	while (!(TWCR &	(1<<TWINT)));
	//Check for ACK
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK){
		return 2;
	}
	
	//Load data address
	TWDR = 0x00;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Wait for reply
	while (!(TWCR &	(1<<TWINT)));
	//Check for ACK
	if ((TWSR & 0xF8) !=TW_MT_DATA_ACK){
		return 3;
	}
	
	//Send repeated start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	//Wait for start condition to be sent
	while (!(TWCR & (1<<TWINT)));
	//Check if repeated start was successful
	if ((TWSR & 0xF8) != TW_REP_START){
		return 4;
	}
	
	//Send slave address in read mode
	TWDR = SLA_R;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Wait for reply
	while (!(TWCR &	(1<<TWINT)));
	//Check for ACK
	if ((TWSR & 0xF8) != TW_MR_SLA_ACK){
		return 5;
	}
	
	for(uint8_t i=0;i<num;i++){
		if(i==num-1){
			//Receive byte with NACK (end)
			TWCR = (1<<TWINT | 1<<TWEN);
			//Wait for reply
			while (!(TWCR &	(1<<TWINT)));
			//Check for ACK
			if ((TWSR & 0xF8) != TW_MR_DATA_NACK){
				return 6;
			}
		}else{
			//Receive byte with ACK (continue)
			TWCR = (1<<TWINT | 1<<TWEN | 1<<TWEA);
			//Wait for reply
			while (!(TWCR &	(1<<TWINT)));
			//Check for ACK
			if ((TWSR & 0xF8) != TW_MR_DATA_ACK){
				return 7;
			}
		}
		RTC_Data[i] = TWDR;
	}
	
	//Send stop
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	return 0;
}

uint8_t RTC_setSeconds(uint8_t s){
	//Send start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	//Wait for start condition to be sent
	while (!(TWCR & (1<<TWINT)));
	//Check if start was sent
	if ((TWSR & 0xF8) != TW_START){
		RTC_error(1);
	}
	
	//Send slave address in write mode
	TWDR = SLA_W;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Wait for reply
	while (!(TWCR &	(1<<TWINT)));
	//Check for ACK
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK){
		RTC_error(2);
	}
	
	//Load data address
	TWDR = 0x00;	//Seconds
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Wait for reply
	while (!(TWCR &	(1<<TWINT)));
	//Check for ACK
	if ((TWSR & 0xF8) !=TW_MT_DATA_ACK){
		RTC_error(3);
	}
	
	//Load data
	TWDR = s;
	TWCR = (1<<TWINT) | (1<<TWEN);
	//Wait for reply
	while (!(TWCR &	(1<<TWINT)));
	//Check for ACK
	if ((TWSR & 0xF8) !=TW_MT_DATA_ACK){
		RTC_error(4);
	}
	
	//Send stop
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	return 0;
}

void RTC_error(uint8_t n){
	digits[0] = D_EMPTY;
	digits[1] = D_EMPTY;
	digits[2] = D_EMPTY;
	digits[3] = D_ERROR;
	digits[4] = n;
}

void RTC_writeTime(){
	digits[0] = (RTC_Data[RTC_MIN]&0xF0)>>4;
	digits[1] = (RTC_Data[RTC_MIN]&0x0F);
	digits[2] = D_DASH;
	digits[3] = (RTC_Data[RTC_SEC]&0xF0)>>4;
	digits[4] = (RTC_Data[RTC_SEC]&0x0F);
}
