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
/* User Global Variable Declaration                                           */
/* variable declard in Main                                                   */
/******************************************************************************/

extern char TempMotorOrdertable[MEAN_TABLE_SIZE];
extern struct MotionOrder UserOrder ;
extern struct MotionOrder MotorOrder ;

/******************************************************************************/


void MotorInitVar(void)
{
    for (char i=0;i<sizeof TempMotorOrdertable;i++ )
    {
        TempMotorOrdertable[i]=0;
    }

}

void MotorMeanComputation(void)
{
    unsigned int value=0;
    unsigned char LocalPowerValue;
    
    // store global variable to local variable for local calculations
    LocalPowerValue = UserOrder.PowerValue;
    
    // if direction requested diferent to actual direction, first go to zero power
    if(UserOrder.direction!=MotorOrder.direction)
    {
        LocalPowerValue=0;
    }
    
    if(UserOrder.direction!=MotorOrder.direction && MotorOrder.PowerValue<REVERSE_TH)
    {
        MotorOrder.direction = UserOrder.direction;
    }
    
    // shift all table
    for(char i=1; i<sizeof TempMotorOrdertable;i++)
    {
    TempMotorOrdertable[(sizeof TempMotorOrdertable)-i]=TempMotorOrdertable[(sizeof TempMotorOrdertable)-i-1];
    }

    // store user value in the table
    TempMotorOrdertable[0]=LocalPowerValue;

    // calculate mean
    for(char i=0; i<sizeof TempMotorOrdertable;i++)
    {
    value+=TempMotorOrdertable[i];
    }

    // store to motor order variable
    MotorOrder.PowerValue=(char) (value/(sizeof TempMotorOrdertable));
    
}


void PutMotorOrder(char value,char direction)
{
    char temp=0;

    CCPR2L=value>>1;
    CCPR3L=value>>1;

    if((value & 0x01)==1)
    {
       CCP2CONbits.DC2B=0b10;
       CCP3CONbits.DC3B=0b10;
    }
    if((value & 0x01)==0)
    {
        CCP2CONbits.DC2B=0b00;
        CCP3CONbits.DC3B=0b10;
    }


    if(direction==CW)
    {
        PH2_H=0;
        CCP2CON = 0;                        // stop modulation on Phi1L

        PH1_L=0;                            //PH1L to 0
        PH1_H=1;                            //PH1H to 1
        
        if(value!=100)
        {
            CCP3CON = CCP_PWM_MODE;         // modulate PH2L if not to 100%
        }
        if(value==100)
        {
            CCP3CON = 0;                    // halt the PWM controller for PH2L
            PH2_L=1;                        // if 100%, don't modulate PH2L

        }
    }

    if(direction==CCW)
    {
        PH1_H=0;
        CCP3CON = 0;

        PH2_L=0;                        // PH2L to 0
        PH2_H=1;                        // PH2H to 1
        
        if(value!=100)
        {
            CCP2CON = CCP_PWM_MODE;         // modulate PH1L if not to 100%
        }

        if(value==100)
        {
            CCP2CON = 0;                    // halt the PWM controller for PH1L
            PH1_L=1;                        // if 100%, don't modulate PH1L
        }
    }

    if(direction==MOTOR_OFF || value==0)
    {
        PH1_H=0;
        PH2_H=0;
        CCP2CON=0;
        CCP3CON=0;
    }

}

