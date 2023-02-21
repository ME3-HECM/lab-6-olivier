#include <xc.h>
#include "rc_servo.h"
#include <pic18f67k40.h>

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
    
    TRISDbits.TRISD5=0; //set TRIS value for pin (output)
    LATDbits.LATD5=0;
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
    if (PIR0bits.TMR0IF)
    {
        if(LATDbits.LATD5){ //if output pin currently high
            write16bitTMR0val(65535-off_period); //set new off_period
            LATDbits.LATD5=0; //turn your output pin off here
        } else {
            write16bitTMR0val(65535-on_period);  //set new on_period
            LATDbits.LATD5=1; //turn your output pin on here
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
    T0CON1bits.T0CKPS=0b0011; // need to work out prescaler to produce a timer tick corresponding to 1 deg angle change (1:8)
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
 * the on_period varies linearly according to angle (-90 deg is 1 ms, +90 is 2 ms)
 * off_period is the remaining time left (calculate from on_period and T_PERIOD)
************************************/
void angle2PWM(unsigned int angle){
    // to oscillate from -90 to 90 degrees
    // ticks corresponding to 1ms = 1/Tint=2000
    //additional ticks per angle time = 1/180 for amount of time for one angle then
    //angletime/Tint= additional ticks for one angle
    //total period =20ms, = on period +off period, off=20-on
    on_period = 3000 + (20*angle);	//avoid floating point numbers and be careful of calculation order...
    off_period = T_PERIOD-on_period;
}