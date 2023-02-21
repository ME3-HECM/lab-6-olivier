#ifndef _RC_servo_H
#define _RC_servo_H

#include <xc.h>

#define _XTAL_FREQ 64000000
//20= Tint *count, we want count i.e. timer ticks
#define T_PERIOD 40000 // enter your total period value here (i.e. timer ticks for 20 ms) (no = sign for #defines)

unsigned int on_period,off_period;

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

void Timer0_init(void);
void write16bitTMR0val(unsigned int);

void angle2PWM(int angle);

#endif
