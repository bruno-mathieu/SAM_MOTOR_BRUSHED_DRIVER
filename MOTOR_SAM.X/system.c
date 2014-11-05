/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "system.h"
#include "define.h"

#endif

/******************************************************************************/
/* Extern global variables declarations                                       */
/******************************************************************************/



/* Refer to the device datasheet for information about available
oscillator configurations. */

void ConfigureOscillator(void)
{

    OSCCONbits.IDLEN=1; /* Iddle mode when SLEEP instruction*/
    OSCCONbits.IRCF=0;  /* don't care*/
    OSCCONbits.SCS=0;   /*Primary oscillator selected*/

}

/* ---------------   GPIO configurations ---------------*/

void ConfigureGPIO(void)
{

    ANCON0 = 0b00000000;    // AN 0-->7 disabled
    ANCON1 = 0b00000010;    // AN 9 enabled for throtle

    ADCON0bits.CHS=0x9;     // an9 selected for analog conversion
    
    ADCON1bits.TRIGSEL=0;
    ADCON1bits.VCFG=0;      // vref + to AVDD
    ADCON1bits.VNCFG=0;     //Vref- to GND
    ADCON1bits.CHSN=0;      //negative channel to GND

    ADCON2bits.ADCS=0x6;    //64Tosc TAD
    ADCON2bits.ACQT=0x7;    //20 TAD acquisition time
    ADCON2bits.ADFM=1;      //result right justified

    ADCON0bits.ADON=1;      // adc module enabled

    TRISA = 0x1F;           // input for ADC inputs

    TRISE = 0b11111110;     // output for EN gate
    LATEbits.LATE0=1;      // EN gate activation

    TRISB = 0b11111011;    // output for TX CAN
    LATBbits.LATB2=0;      // fix output TXCAN


    TRISC = 0b00010000;    // outputs all, except SPI_MISO
    LATCbits.LATC1=1;      // SPI CS to 1
    LATCbits.LATC2=0;      // PHI1_L
    LATCbits.LATC3=0;      // SPI_CLK to 0
    LATCbits.LATC5=0;      // SPI_MOSI to 0
    LATCbits.LATC6=0;      // PH2_L to 0
    LATCbits.LATC7=0;      // PH3_L to 0

    TRISD = 0b00000111;    // output for phases and DC cal
    LATDbits.LATD3=0;      // phi1_H to 0
    LATDbits.LATD4=0;      // phi2_H to 0
    LATDbits.LATD5=0;      // phi3_H to 0
    LATDbits.LATD6=0;      // DC_CAL to 0

    INTCON2bits.nRBPU=1;   // disable pull-up on portb

    WPUB=0x00;             // pullups disabled on button inputs

    PADCFG1bits.RDPU=1;     // enable pull ups on PORTD ( for MOS driver fault outputs.)
    
}

void ConfigureInterrupts(void)
{
    //all interrupts are low priority, except CAN module

    RCONbits.IPEN = 1;           // enables priority logic for Interrupts.
            
    INTCON = 0b00100000;        // GIE interrupts disabled for the moment, TMRO OVF activated
    INTCON2 = 0b11111000;       // low priority for TMR0 overflow
    INTCON3 = 0b00000000;       // no external interrupt

    PIR1 = 0;                   //clear IT flags
    PIR2 = 0;
    PIR3 = 0;
    PIR4 = 0;
    PIR5 = 0;

    PIE1 = 0;          
    PIE2 = 0;
    PIE3 = 0;
    PIE4 = 0;
    PIE5 = 0b00000011;          // enable CAN RX interrupts.

    IPR1= 0 ;                   // low priority IT's
    IPR2= 0 ;                   // low priority IT's
    IPR3= 0 ;                   // low priority IT's
    IPR4= 0 ;                   // low priority IT's
    IPR5= 0b00000011 ;          // High priority for CAN RX.
   
    INTCONbits.GIE=1;           // general IT enable
    INTCONbits.PEIE=1;          // peripheral IT enable
}

void ConfigureTimers(void)
{
    // timer0 used for 10ms tick, in main application

    T0CONbits.T08BIT = 0;       //TMR0 in 16 bits mode
    T0CONbits.T0CS = 0;         // TMRO source is internal clock
    T0CONbits.PSA = 0;          // PSA assigned to TIMER0
    T0CONbits.T0PS=6;           // 1:128 prescaler

    TMR0H = TMR0H_INIT;
    TMR0L = TMR0L_INIT;         // preload for 10ms overflow (TMR0 = 1250)

    T0CONbits.TMR0ON = 1;       // enable TIMER0

    // timer 2: used for low side PWM transistors

    T2CONbits.T2OUTPS = 0;        // 1/1 postscaler
    T2CONbits.T2CKPS = 0x2;       // 1/16 prescaler
    PR2 = 50;                    // period register for use with motor PWM (frequency 20kHz)

    
  
}

void ConfigureCCP(void)
{
    CCP2CON = 0;         // CCP2 disabled at this time
    CCP3CON = 0;         // CCP3 disabled at this time
    CCP4CON = 0;         // CCP4 disabled at this time
    CCP5CON = 0;         // CCP5 not used
    
    CCPTMRS= 0b00000001;    // CCP5 based on Tmr2 
                            // CCP2 based on tmr2
                            // ECCP1 based on timer 4 (but not used)
    
    T2CONbits.TMR2ON=1;     // timer 2 activation

}

char GetThrotle(void)
{
    unsigned int adc_result;

    ADCON0bits.GO=1;
    while(ADCON0bits.GO==1);
    adc_result=(ADRESH);
    adc_result=adc_result<<8;
    adc_result=adc_result | ADRESL;

    if(adc_result<THROTTLE_OFFSET)
    {
        adc_result=THROTTLE_OFFSET;
    }

    if(adc_result>THROTTLE_MAX_VALUE)
    {
        adc_result=THROTTLE_MAX_VALUE;
    }

    adc_result=(adc_result-THROTTLE_OFFSET)/THROTTLE_DIVIDER;

    if(adc_result<15)
        adc_result=0;
    if(adc_result>85)
        adc_result=100;

    return (char) adc_result;

}

char GetKeyCode(void)
{
    char temp;
    
    temp = PORTB;
    temp=temp & 0b00110000;
    
    return temp;
}

char GetCanAdress(void)
{
    return 0;
}

char SystemTest(void)
{
    return TRUE;
}