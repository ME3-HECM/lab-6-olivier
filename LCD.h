#ifndef _LCD_H
#define _LCD_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

//Define Pins Used with the LCD Screen
#define LCD_RS LATCbits.LATC6
#define LCD_E LATCbits.LATC2
#define LCD_DB4 LATBbits.LATB3
#define LCD_DB5 LATBbits.LATB2
#define LCD_DB6 LATEbits.LATE3
#define LCD_DB7 LATEbits.LATE1
/* Display ON/OFF Control */
#define DON         0b00001111  /* Display on      */
#define DOFF        0b00001011  /* Display off     */
#define CURSOR_ON   0b00001111  /* Cursor on       */
#define CURSOR_OFF  0b00001101  /* Cursor off      */
#define CURSOR_RESET 0b00000010  /* Cursor on       */
#define BLINK_ON    0b00001111  /* Cursor Blink    */
#define BLINK_OFF   0b00001110  /* Cursor No Blink */
#define DCLEAR      0b00000001  /* Clear Display */
/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT    0b00010011  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   0b00010111  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   0b00011000  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  0b00011100  /* Display shifts to the right */

void LCD_E_TOG(void);
void LCD_sendnibble(unsigned char number);
void LCD_sendbyte(unsigned char Byte, char type);
void LCD_Init(void);
void LCD_setline (char line);	
void LCD_sendstring(char *string);
void LCD_scroll(void);
void LCD_clear(void);
void ADC2String(char *buf,unsigned int number);

#endif
