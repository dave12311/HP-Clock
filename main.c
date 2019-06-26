/*
 * HP Clock.c
 *
 * Created: 6/17/2019 3:53:55 PM
 * Author : Dave
 */
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include <stdint.h>

#include "Libs/control.h"
#include "Libs/RTC.h"
#include "Libs/DHT11.h"

int main(void) {
    control_init();
    RTC_init();

    uint8_t err = 0;

    startSetup();
    RTC_setData();

    uint8_t count = 0;

    while (1) {
        if (count == 255) {
            err = RTC_getData(7);
        }

        if (err == 0) {
            if (displayMode == DISPLAY_MODE_TIME) {
                displayTime();
            } else if (displayMode == DISPLAY_MODE_DATE) {
                displayDate();
            } else if (displayMode == DISPLAY_MODE_YEAR) {
                displayYear();
            }
        } else {
            RTC_error(err);
            _delay_ms(10000);
        }
        _delay_ms(10);
        count++;
    }
}
