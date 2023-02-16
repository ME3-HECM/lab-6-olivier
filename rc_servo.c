#include <xc.h>
#include "rc_servo.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * individual interrupt sources are enabled in their respective module init functions
************************************/
void Interrupts_init(void)
{
    PIE0bits.TMR0IE=1;  //interrupt on timer0 overflow (or match in 8bit mode)
    INTCONbits.PEIE=1;  //peripheral interrupts enabled (controls anything in PIE1+)
    INTCONbits.IPEN=0;  //high priority only
    INTCONbits.GIE=1;   //global interrupt enabled
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
    if (PIR0bits.TMR0IF)
    {
        if(LAT?bits.???){ //if output pin currently high
            write16bitTMR0val(65535-off_period); //set new off_period
            LAT?bits.???=0; //turn your output pin off here
        } else {
            write16bitTMR0val(65535-on_period);  //set new on_period
            LAT?bits.???=1; //turn your output pin off here
        }
    }
    PIR0bits.TMR0IF=0; 
}

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=????; // need to work out prescaler to produce a timer tick corresponding to 1 deg angle change
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    // it's a good idea to initialise the timer so that it initially overflows after 20 ms
    TMR0H=(65535-T_PERIOD)>>8;            
    TMR0L=(unsigned char)(65535-T_PERIOD); // casting to unsigned char here to suppress warning
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * Function to write a full 16bit timer value
 * Note TMR0L and TMR0H must be written in the correct order, or TMR0H will not contain the correct value
************************************/
void write16bitTMR0val(unsigned int tmp)
{
    TMR0H=tmp>>8; //MSB written
    TMR0L=tmp; //LSB written and timer updated
}

/************************************
 * Function to set the on_period and off_period variables used in the ISR
 * the on_period varies linearly according to angle (-90 deg is 0.5 ms, +90 is 2.1 ms)
 * off_period is the remaining time left (calculate from on_period and T_PERIOD)
************************************/
void angle2PWM(int angle){
    on_period = ???;	//avoid floating point numbers and be careful of calculation order...
    off_period = ???;
}