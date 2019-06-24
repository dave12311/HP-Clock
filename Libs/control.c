/*
 * control.c
 *
 * Created: 6/17/2019 7:58:12 PM
 *  Author: Dave
 */
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "control.h"
#include "bitop.h"
#include "RTC.h"

//Display data
const uint8_t Digit_data[DIGIT_NUM] = {
        //   0           1           2           3           4
        0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110,
        //   5           6            7          8           9
        0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110,
        //               E           -
        0b00000000, 0b10011110, 0b00000010
};

ISR(TIMER0_OVF_vect) {
    //Update screen
    if (blinkMode == BLINK_MODE_ALL) {
        if (blink == 1) {
            sh_shiftDigit(digits[digitCounter]);
        } else {
            sh_shiftDigit(D_EMPTY);
        }
    } else if (blinkMode == BLINK_MODE_RIGHT) {
        if (digitCounter != 3 && digitCounter != 4) {
            sh_shiftDigit(digits[digitCounter]);
        } else if (blink == 1) {
            sh_shiftDigit(digits[digitCounter]);
        } else {
            sh_shiftDigit(D_EMPTY);
        }
    } else if (blinkMode == BLINK_MODE_LEFT) {
        if (digitCounter != 0 && digitCounter != 1) {
            sh_shiftDigit(digits[digitCounter]);
        } else if (blink == 1) {
            sh_shiftDigit(digits[digitCounter]);
        } else {
            sh_shiftDigit(D_EMPTY);
        }
    } else {
        if (digitCounter != 2) {
            sh_shiftDigit(digits[digitCounter]);
        } else if (blink == 1) {
            sh_shiftDigit(digits[digitCounter]);
        } else {
            sh_shiftDigit(D_EMPTY);
        }
    }
    switchCathode(digitCounter++);
    if (digitCounter == 5) { digitCounter = 0; }

    //Poll encoder
    enc_button = (PINB & 0x1);
    enc_a = (PINB & 0x2) >> 1;
    enc_b = (PINB & 0x4) >> 2;

    if (!enc_a && !enc_b && enc_b_old) {
        if (encoderEventRight != 0) {
            resetBlink();
            (*encoderEventRight)();
        }
    } else if (!enc_a && enc_b && !enc_b_old) {
        if (encoderEventLeft != 0) {
            resetBlink();
            (*encoderEventLeft)();
        }
    }

    //Check button state
    if (!enc_button && enc_button_old) {
        encButtonPressed = 1;
    }

    enc_a_old = enc_a;
    enc_b_old = enc_b;
    enc_button_old = enc_button;

}

ISR(TIMER1_OVF_vect) {
    if (blink == 0) {
        blink = 1;
    } else {
        blink = 0;
    }
}

void control_init(void) {
    //Set default values
    blink = 0;
    blinkMode = 0;
    digitCounter = 0;
    enc_a = 1;
    enc_b = 1;
    enc_a_old = 1;
    enc_b_old = 1;
    enc_button = 1;
    enc_button_old = 1;
    encButtonPressed = 0;
    encoderEventRight = 0;
    encoderEventLeft = 0;

    //Set shift register pins as outputs
    setBit(SH_DDR, SH_DATA);
    setBit(SH_DDR, SH_SHIFT_CLOCK);
    setBit(SH_DDR, SH_STORAGE_CLOCK);
    setBit(SH_DDR, SH_OE);

    //Enable internal pull-up for encoder pins
    setBit(PORTB, 0);
    setBit(PORTB, 1);
    setBit(PORTB, 2);

    //Setup Timer 0
    TCCR0B |= 0x3;
    TIMSK0 |= 0x1;    //Enable overflow interrupt

    //Setup Timer 1 [BLINK]
    TCCR1B |= 0x3;

    //Enable global interrupts
    sei();
}

void sh_pulseSH(void) {
    setBit(SH_PORT, SH_SHIFT_CLOCK);
    clearBit(SH_PORT, SH_SHIFT_CLOCK);
}

void sh_pulseST(void) {
    setBit(SH_PORT, SH_STORAGE_CLOCK);
    clearBit(SH_PORT, SH_STORAGE_CLOCK);
}

void sh_shiftDigit(uint8_t d) {
    for (uint8_t i = 0; i < 8; i++) {
        changeBit(SH_PORT, SH_DATA, (Digit_data[d] & (1 << i)) >> i);
        sh_pulseSH();
    }
    sh_pulseST();
}

void switchCathode(uint8_t c) {
    //Reset cathodes
    CAT_DDR &= ~(0b11111000);

    //Set active cathode
    CAT_DDR |= (1 << (c + 3));
}

void blinkOn(void) {
    //Enable interrupt
    TIMSK1 = 0x1;
}

void blinkOff(void) {
    //Disable interrupt
    TIMSK1 = 0x0;
    blink = 1;
}

void resetBlink(void) {
    blink = 1;
    TCNT1H = 0;
    TCNT1L = 0;
}

void startSetup(void) {
    blinkMode = BLINK_MODE_ALL;
    blinkOn();
    //YEAR
    RTC_Data[RTC_YEAR] = 0;
    digits[0] = D_EMPTY;
    digits[1] = 2;
    digits[2] = 0;
    digits[3] = 0;
    digits[4] = 0;
    encoderEventRight = &EventYearRight;
    encoderEventLeft = &EventYearLeft;
    while (!encButtonPressed);
    encButtonPressed = 0;

    //DATE
    blinkMode = BLINK_MODE_LEFT;
    RTC_Data[RTC_MONTH] = 0;
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = D_DASH;
    digits[3] = 0;
    digits[4] = 0;
    encoderEventRight = &EventMonthRight;
    encoderEventLeft = &EventMonthLeft;
    while (!encButtonPressed);
    encButtonPressed = 0;

    blinkMode = BLINK_MODE_RIGHT;
    RTC_Data[RTC_DATE] = 0;
    encoderEventRight = &EventDateRight;
    encoderEventLeft = &EventDateLeft;
    while (!encButtonPressed);
    encButtonPressed = 0;

    //TIME
    blinkMode = BLINK_MODE_LEFT;
    RTC_Data[RTC_HOUR] = 0;
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = D_DASH;
    digits[3] = 0;
    digits[4] = 0;
    encoderEventRight = &EventHourRight;
    encoderEventLeft = &EventHourLeft;
    while (!encButtonPressed);
    encButtonPressed = 0;

    blinkMode = BLINK_MODE_RIGHT;
    RTC_Data[RTC_MIN] = 0;
    encoderEventRight = &EventMinRight;
    encoderEventLeft = &EventMinLeft;
    while (!encButtonPressed);
    encButtonPressed = 0;

    blinkMode = BLINK_MODE_MIDDLE;
    encoderEventRight = 0;
    encoderEventLeft = 0;
}

void EventYearRight(void) {
    if (RTC_Data[RTC_YEAR] == 0x99) {
        RTC_Data[RTC_YEAR] = 0;
        digits[3] = 0;
        digits[4] = 0;
    } else if ((RTC_Data[RTC_YEAR] & 0x0F) == 9) {
        RTC_Data[RTC_YEAR] += 0x7;
        digits[3]++;
        digits[4] = 0;
    } else {
        RTC_Data[RTC_YEAR]++;
        digits[4]++;
    }
}

void EventYearLeft(void) {
    if (RTC_Data[RTC_YEAR] == 0x00) {
        RTC_Data[RTC_YEAR] = 0x99;
        digits[3] = 9;
        digits[4] = 9;
    } else if ((RTC_Data[RTC_YEAR] & 0x0F) == 0) {
        RTC_Data[RTC_YEAR] -= 0x7;
        digits[3]--;
        digits[4] = 9;
    } else {
        RTC_Data[RTC_YEAR]--;
        digits[4]--;
    }
}

void EventMonthRight(void) {
    if (RTC_Data[RTC_MONTH] == 0x12) {
        RTC_Data[RTC_MONTH] = 0;
        digits[0] = 0;
        digits[1] = 0;
    } else if ((RTC_Data[RTC_MONTH] & 0x0F) == 9) {
        RTC_Data[RTC_MONTH] += 0x7;
        digits[0]++;
        digits[1] = 0;
    } else {
        RTC_Data[RTC_MONTH]++;
        digits[1]++;
    }
}

void EventMonthLeft(void) {
    if (RTC_Data[RTC_MONTH] == 0x00) {
        RTC_Data[RTC_MONTH] = 0x12;
        digits[0] = 1;
        digits[1] = 2;
    } else if ((RTC_Data[RTC_MONTH] & 0x0F) == 0) {
        RTC_Data[RTC_MONTH] -= 0x7;
        digits[0]--;
        digits[1] = 9;
    } else {
        RTC_Data[RTC_MONTH]--;
        digits[1]--;
    }
}

void EventDateRight(void) {
    if (RTC_Data[RTC_DATE] == 0x31) {
        RTC_Data[RTC_DATE] = 0;
        digits[3] = 0;
        digits[4] = 0;
    } else if ((RTC_Data[RTC_DATE] & 0x0F) == 9) {
        RTC_Data[RTC_DATE] += 0x7;
        digits[3]++;
        digits[4] = 0;
    } else {
        RTC_Data[RTC_DATE]++;
        digits[4]++;
    }
}

void EventDateLeft(void) {
    if (RTC_Data[RTC_DATE] == 0x00) {
        RTC_Data[RTC_DATE] = 0x31;
        digits[3] = 3;
        digits[4] = 1;
    } else if ((RTC_Data[RTC_DATE] & 0x0F) == 0) {
        RTC_Data[RTC_DATE] -= 0x7;
        digits[3]--;
        digits[4] = 9;
    } else {
        RTC_Data[RTC_DATE]--;
        digits[4]--;
    }
}


void EventHourRight(void) {
    if (RTC_Data[RTC_HOUR] == 0x23) {
        RTC_Data[RTC_HOUR] = 0;
        digits[0] = 0;
        digits[1] = 0;
    } else if ((RTC_Data[RTC_HOUR] & 0x0F) == 9) {
        RTC_Data[RTC_HOUR] += 0x7;
        digits[0]++;
        digits[1] = 0;
    } else {
        RTC_Data[RTC_HOUR]++;
        digits[1]++;
    }
}

void EventHourLeft(void) {
    if (RTC_Data[RTC_HOUR] == 0x00) {
        RTC_Data[RTC_HOUR] = 0x23;
        digits[0] = 2;
        digits[1] = 3;
    } else if ((RTC_Data[RTC_HOUR] & 0x0F) == 0) {
        RTC_Data[RTC_HOUR] -= 0x7;
        digits[0]--;
        digits[1] = 9;
    } else {
        RTC_Data[RTC_HOUR]--;
        digits[1]--;
    }
}

void EventMinRight(void) {
    if (RTC_Data[RTC_MIN] == 0x59) {
        RTC_Data[RTC_MIN] = 0;
        digits[3] = 0;
        digits[4] = 0;
    } else if ((RTC_Data[RTC_MIN] & 0x0F) == 9) {
        RTC_Data[RTC_MIN] += 0x7;
        digits[3]++;
        digits[4] = 0;
    } else {
        RTC_Data[RTC_MIN]++;
        digits[4]++;
    }
}

void EventMinLeft(void) {
    if (RTC_Data[RTC_MIN] == 0x00) {
        RTC_Data[RTC_MIN] = 0x59;
        digits[3] = 5;
        digits[4] = 9;
    } else if ((RTC_Data[RTC_MIN] & 0x0F) == 0) {
        RTC_Data[RTC_MIN] -= 0x7;
        digits[3]--;
        digits[4] = 9;
    } else {
        RTC_Data[RTC_MIN]--;
        digits[4]--;
    }
}

void displayTime(void) {
    digits[0] = (RTC_Data[RTC_HOUR] & 0xF0) >> 4;
    digits[1] = (RTC_Data[RTC_HOUR] & 0x0F);
    digits[2] = D_DASH;
    digits[3] = (RTC_Data[RTC_MIN] & 0xF0) >> 4;
    digits[4] = (RTC_Data[RTC_MIN] & 0x0F);
}

void displayDate(void) {
    digits[0] = (RTC_Data[RTC_MONTH] & 0xF0) >> 4;
    digits[1] = (RTC_Data[RTC_MONTH] & 0x0F);
    digits[2] = D_DASH;
    digits[3] = (RTC_Data[RTC_DATE] & 0xF0) >> 4;
    digits[4] = (RTC_Data[RTC_DATE] & 0x0F);
}



