// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "rc_servo.h"
#include "ADC.h"
#include <stdio.h>
#include "LCD.h"
#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Timer0_init();
    Interrupts_init();
    ADC_init();
    LCD_Init();
    LCD_setline(0); //Set Line low=1 line,high=2nd line
    while(1){
             char LDR[3];
        //check if it is bright outside
        if (ADCDarkorLight()&1){
            //if so simulate  open window w servo
            angle2PWM(90);
            __delay_ms(50);
        }
       //check if it is dark outside
        if (ADCDarkorLight()&0){
        //if so simulate closed window w servo
            angle2PWM(-90);
              __delay_ms(50);
                }
        ADC2String(LDR,ADC_getval());
        LCD_sendstring(LDR);
        __delay_ms(500);
       LCD_sendbyte(DCLEAR,0);
    }
}
