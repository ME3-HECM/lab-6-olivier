// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "rc_servo.h"
#include "LCD.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Timer0_init();
    Interrupts_init();
    //LCD_Init();
    LCD_setline(0);
    char ar[20];
	//don't forget TRIS for your output!
    TRISDbits.TRISD5=0; 
    while(1){
		//write your code to call angle2PWM() to set the servo angle
        //runs 180 times incrementing the angle each time 
        int x;
        for (x=0;x<180;x++){
            angle2PWM(x);
            ADC2String(ar,x);
            LCD_sendstring(ar);
            __delay_ms(50);
            
        }

    }
}
