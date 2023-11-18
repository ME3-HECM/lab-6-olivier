#include <xc.h>
#include <pic18f67k40.h>
#include "LCD.h"
#include <stdio.h>

/************************************
 * Function to toggle LCD enable bit on then off
 * when this function is called the LCD screen reads the data lines
************************************/
void LCD_E_TOG(void)
{
	//turn the LCD enable bit on
    LATCbits.LATC2=1;
	__delay_us(30); //wait a short delay
	//turn the LCD enable bit off again
    LATCbits.LATC2=0;
}

/************************************
 * Function to set the 4-bit data line levels for the LCD
************************************/
void LCD_sendnibble(unsigned char number)
{
	//set the data lines here (think back to LED array output)
    if (0b1000&number) {LATEbits.LATE1=1;} else{LATEbits.LATE1=0;}
    if (0b0100&number) {LATEbits.LATE3=1;} else{LATEbits.LATE3=0;}
    if (0b0010&number) {LATBbits.LATB2=1;} else{LATBbits.LATB2=0;}
    if (0b0001&number) {LATBbits.LATB3=1;} else{LATBbits.LATB3=0;}
    LCD_E_TOG();			//toggle the enable bit to instruct the LCD to read the data lines
    __delay_us(5);      //Delay 5uS
}


/************************************
 * Function to send full 8-bit commands/data over the 4-bit interface
 * high nibble (4 most significant bits) are sent first, then low nibble sent
************************************/
void LCD_sendbyte(unsigned char Byte, char type)
{
    // set RS pin whether it is a Command (0) or Data/Char (1) using type argument
    LATCbits.LATC6=type;
    // send high bits of Byte using LCDout function
    char high_Byte= (Byte>>4) ;
    LCD_sendnibble(high_Byte);
    // send low bits of Byte using LCDout function
    LCD_sendnibble(Byte);
	
    __delay_us(60);               //delay 50uS (minimum for command to execute)
}

/************************************
 * Function to initialise the LCD after power on
************************************/
void LCD_Init(void)
{
    //Define LCD Pins as Outputs and
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB2=0;
    TRISEbits.TRISE3=0;
    TRISEbits.TRISE1=0;
    
    //Enable pins as digital 
    ANSELBbits.ANSELB2=0;
    ANSELBbits.ANSELB3=0;
    ANSELEbits.ANSELE3=0;
    ANSELEbits.ANSELE1=0;
    //Set up enable pin as an output
    TRISCbits.TRISC2=0;
    //SET UP RS PIN as output
    TRISCbits.TRISC6=0;
    //set all pins low (might be random values on start up, fixes lots of issues)
    LATBbits.LATB3=0;
    LATBbits.LATB2=0;
    LATEbits.LATE3=0;
    LATEbits.LATE1=0;
    LATCbits.LATC6=0;
    LATCbits.LATC2=0;

    //Initialisation sequence code
    __delay_ms(50);
	// follow the sequence in the GitHub Readme picture for 4-bit interface.
	// first Function set should be sent with LCD_sendnibble (the LCD is in 8 bit mode at start up)
    //send 3 times for intialisation
    LCD_sendnibble(0b0011);
    LCD_sendnibble(0b0011);
    LCD_sendnibble(0b0011);
    __delay_ms(5);
	// after this use LCD_sendbyte to operate in 4 bit mode
    LCD_sendbyte(0b00100000,0);
    __delay_ms(1);
    //set the amount of lines in the 5th most sig bit, high= 2 lies low=1 line
    LCD_sendbyte(0b00100000,0);
    __delay_ms(1);
    LCD_sendbyte(0b00001000,0);
    __delay_ms(1);
    LCD_sendbyte(0b00000001,0);
    __delay_ms(5);
    LCD_sendbyte(0b00000110,0);
        __delay_ms(5);
	//remember to turn the LCD display back on at the end of the initialisation (not in the data sheet)
    //The last 4 bytes are *DCB where D= display on or off C is cursor on or off
        //and B is blink or not
    LCD_sendbyte(0b00001111,0);
    __delay_ms(1);
}

/************************************
 * Function to set the cursor to beginning of line 1 or 2
************************************/
void LCD_setline (char line)
{
    //Send 0x80 to set line to 1 (0x00 ddram address)
    if (line&0b0000) {LCD_sendbyte(0x80,0);}
    //Send 0xC0 to set line to 2 (0x40 ddram address)
    if (line&0b0001) {LCD_sendbyte(0xC0,0);}
}

/************************************
 * Function to send string to LCD screen
 * This looks at each charACTER in the string
 * since a string is stored as an array of it's binary value can send it directly
 * to the LCD to be printed
************************************/
void LCD_sendstring(char *string)
{
    // This counts the amount of spaces used 
    int count=0;
    while(*string != 0){  // While the data pointed to isn't a 0x00 do below (strings in C must end with a NULL byte) 
        //If reached end of screen scroll to next part for each dditional letter
        if (count>=16){ 
            //pause message so it can be read before scrolling
           if (count&16) {__delay_ms(1000);}
           LCD_scroll();
           __delay_ms(2);
        }
        //When moving to the next line(after max 40 char reached reset count)
        if (count>=40){
            count=0;}
        	//Send out the current byte pointed to and increment the pointer
		LCD_sendbyte(*string++,1); 
        __delay_us(20);
        count++;
	}
}

/************************************
 * Function to send string to LCD screen
************************************/
void LCD_scroll(void)
{
	//code here to scroll the text on the LCD screen
    LCD_sendbyte(SHIFT_DISP_LEFT,0);
    __delay_us(40);
}

/************************************
 * Function takes a ADC value and works out the voltage to 2 dp
 * the result is stored in buf as ascii text ready for display on LCD
 * Note result is stored in a buffer using pointers, it is not sent to the LCD
************************************/
void ADC2String(char *buf,unsigned int ADC_val){
	//code to calculate the inegeter and fractions part of a ADC value
    int int_part= (ADC_val)/77.27; //i.e 255/3.3v= 77.27....
    int frac_part=  (100*ADC_val)/77.27 -int_part*100;
	// and format as a string using sprintf (see GitHub readme)
    sprintf(buf," %d Volts",ADC_val);
}
