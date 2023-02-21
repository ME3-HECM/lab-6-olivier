// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "rc_servo.h"
#include "LCD.h"
#include "ADC.h"
#include "comparator.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Timer0_init();
    Interrupts_init();
    ADC_init();
    Comp1_init();
    DAC_init();
    while(1){
 
		//write your code to call angle2PWM() to set the servo angle
        //runs 180 times incrementing the angle each time 

        int LDR = ADC_getval();
        if (LDR<50)//if the light is low open the blind
        {
            angle2PWM(90);
        }
        if (LDR>150)//if the light is high close the blind
        {
            angle2PWM(-90);
        }

    }
}
