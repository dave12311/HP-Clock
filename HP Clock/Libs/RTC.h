/*
 * RTC.h
 *
 * Created: 6/19/2019 11:08:14 PM
 *  Author: horva
 */ 
#include <stdint.h>

#ifndef RTC_H_
#define RTC_H_

#define SLA_W 0b11010000
#define SLA_R 0b11010001

//Seconds, Minutes, Hour, Day, Date, Month, Year
uint8_t RTC_Data[7];

#define RTC_SEC   0
#define RTC_MIN   1
#define RTC_HOUR  2
#define RTC_DAY   3
#define RTC_DATE  4
#define RTC_MONTH 5
#define RTC_YEAR  6

void RTC_init();
uint8_t RTC_getData(uint8_t num);
uint8_t RTC_setData();
void RTC_error(uint8_t n);
void RTC_writeTime();


#endif /* RTC_H_ */