#include <xc.h>
#include "dc_motor.h"

// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(unsigned int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    TRISEbits.TRISE2=0; //output on RE2
    TRISCbits.TRISC7=0; //output on RC7
    TRISEbits.TRISE4=0; //output on RE2
    TRISGbits.TRISG6=0; //output on RC7
    
    LATEbits.LATE2=0; // 0 output on RE2
    LATCbits.LATC7=0; // 0 output on RC7
    LATEbits.LATE4=0; // 0 output on RE4
    LATGbits.LATG6=0; // 0 output on RG6
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b100; // 1:64 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    T2PR=PWMperiod; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(struct DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=(m->PWMperiod) - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=(m->PWMperiod); //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL, struct DC_motor *mR)
{
    while((mL->power)>0 && (mR->power)>0)//whilst the power is non zero 
    {
        //individually check the power to each motor is above zero 
        if (mL->power>0){
            mL->power--;//decrement power       
        }
        if ((mR->power)>0){
            mR->power--; //decrement power
        }
        setMotorPWM(mL);//set new motor power values
        setMotorPWM(mR);
        __delay_us(400);//delay to decrease power slowly
    }
}

//function to make the robot turn left
void turnLeft(struct DC_motor *mL, struct DC_motor *mR)
{
    (mL->direction) = 0; //set motor direction backward for left
    (mR->direction) = 1; //set motor direction forward for right
    setMotorPWM(mL);//set new motor direction 
    setMotorPWM(mR);
    for (unsigned int i = 0; i <50; i++)//increase power value up to 50 (this value is tunable for how fast you want it to go)
    {
        mL->power++; //increase the power to both motors slowly
        mR->power++;
        setMotorPWM(mL);//set new motor power values
        setMotorPWM(mR);
        __delay_us(400);//delay to increase power slowly
    } 
}

//function to make the robot turn right 
void turnRight(struct DC_motor *mL, struct DC_motor *mR)
{
    (mL->direction) = 1; //set motor direction forward for left
    (mR->direction) = 0; //set motor direction backward for right
    setMotorPWM(mL);//set new motor direction 
    setMotorPWM(mR);
    for (unsigned int i = 0; i <50; i++)//increase power value up to 50 (this value is tunable for how fast you want it to go)
    {
        mL->power++; //increase the power to both motors slowly
        mR->power++;
        setMotorPWM(mL);//set new motor power values
        setMotorPWM(mR);
        __delay_us(400);//delay to increase power slowly
    } 
}

//function to make the robot go straight
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{
 (mL->direction) = 1; //set motor direction forward for left
 (mR->direction) = 1; //set motor direction forward for right 
 //here we will have a for loop to increase the power to the motor gradually
    for (unsigned int i = 0; i <50; i++)//increase power value up to max (defined at the top) (this value is tunable for how fast you want it to go)
    {
        (mL->power)++; //increase the power to both motors slowly
        (mR->power)++;
        setMotorPWM(mL);//set new motor power values
        setMotorPWM(mR);
        __delay_us(400);//delay to increase power slowly
    } 
}

//function to turn left 90 degrees
void left90(struct DC_motor *mL, struct DC_motor *mR)
{
    int x;
    //this has been tuned to invoke a turn enough times for a 90 degree change
    for (x=0;x<75;x++){
    turnLeft(mL,mR);//invoke the turn left
    }
    stop(mL,mR);//stop the rotation of the buggy 
    __delay_ms(800);
}

//function to turn right 90 degrees
void right90(struct DC_motor *mL, struct DC_motor *mR)
{    int x;
 //this has been tuned to invoke a turn enough times for a 90 degree change
    for (x=0;x<75;x++){
    turnRight(mL,mR);//invoke the turn right
  }
    stop(mL,mR);//stop the rotation of the buggy 
    __delay_ms(800);
}

void rotate180right(struct DC_motor *mL, struct DC_motor *mR)
{
    turnRight(mL,mR);//invoke the turn right
    __delay_ms(10);//tune this so it corresponds to a 180 degree change
    stop(mL,mR);//stop the rotation of the buggy 
}