/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "define.h"        /* board level definitions*/
#include "ecanpoll.h"      /* CAN library header file*/
#include "Can_HL.h"
#include "motor.h"


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

// structure used to count 10ms tick
unsigned char msTickCounter;

//  variable for CAN  FIFO buffer and CAN adress
struct CANTxFifo CANTxFifo;
struct CANRxFifo CANRxFifo;
char LocalCanAdress;
char MotorTypeIdentifierAdress;      // left or right motor adress

// variables used for motor

char TempMotorOrdertable[MEAN_TABLE_SIZE];
struct MotionOrder UserOrder ;
struct MotionOrder MotorOrder ;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    char TempVar;
    unsigned int TempVarInt;

    char OldKeyCode;
    char KeyEvent;
    char OrderLocked=FALSE;
    char ThrottlePosition;

    // Autotest result
    char AutotestResult;
    
    //  variable for CAN TX FIFO buffer
    struct CANTxMsg TempCANTxMsg;

    //  variable for CAN RX FIFO buffer
    struct CANRxMsg TempCANRxMsg;


    //----------------------------------------------------
    //----------  CPU internal configurations: -----------
    //----------------------------------------------------

    /* Configure the oscillator for the CPU */
    ConfigureOscillator();
    __delay_ms(10);             // wait for Oscillator to be stabilized
    
    // configure CPU GPIO for  board
    ConfigureGPIO();

    //Get can adress from rotary switch
    LocalCanAdress = GetCanAdress();
    //CAN controller Initialize
    //ECANInitialize();
    //Set MASK and Filters for CAN
    //ECANFiltersInit();

    // Timers configuration
    ConfigureTimers();

    // PWM outputs configuration
    ConfigureCCP();

    //----------------------------------------------------
    //----------  Global variables initialisation --------
    //----------------------------------------------------

    // used for 10ms tick
    msTickCounter=FALSE;

     // initialize CAN tx FIFO
    CANTxFifoInit();
    CANRxFifoInit();

    // initialise Motor table variable
    MotorInitVar();
    UserOrder.PowerValue=0;
    MotorOrder.PowerValue=0;

    OldKeyCode=GetKeyCode();
    KeyEvent=FALSE;

    //----------------------------------------------------
    //------  external peripheral configurations: --------
    //----------------------------------------------------

    __delay_ms(10);              // wait for reset to be released on external peripherals
   
    //----------------------------------------------------
    //----------    Ready to go in main loop:  -----------
    //------   interrupts activation and system test -----
    //----------------------------------------------------

    ConfigureInterrupts();

    AutotestResult = SystemTest();
    
    //-----------------------------------------------------
    //-------------  infinite main loop ----------
    //----------------------------------------------------

    PutMotorOrder(0,MOTOR_OFF);     // set motor OFF

//    UserOrder.PowerValue=100;                  // set user order to 100% for test
//    UserOrder.direction=CW;                     // clockwise direction for test

    while(1)
    {
       
        //--------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------
        //-------------  periodic tasks --------------------------------------------------
        //--------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------
        //--------------------------------------------------------------------------------

        //--------------------------------------------------------------------------------
        //-------------                 10ms tick tasks                         ----------
        //--------------------------------------------------------------------------------

        if(msTickCounter==TRUE)
        {
            MotorMeanComputation();
            msTickCounter=FALSE;
            PutMotorOrder(MotorOrder.PowerValue,MotorOrder.direction);

            ThrottlePosition=GetThrotle();

            // if no key is pressed, and order is not locked,
            if(GetKeyCode()==KEY_NONE && OrderLocked==FALSE)
            {
                UserOrder.PowerValue=ThrottlePosition;
                UserOrder.direction=CW;
            }

            // if order is locked, and throttle position <20%, release speed regulator

            if(OrderLocked==TRUE && ThrottlePosition<SPEED_REGULATOR_THRESHOLD)
            {
                UserOrder.PowerValue=ThrottlePosition;
                UserOrder.direction=CW;
                OrderLocked=FALSE;
            }

            // if in reverse mode or motor order =0, and reverse key pressed
            if(GetKeyCode()==KEY_REV && (UserOrder.direction==CCW || UserOrder.PowerValue==0))
            {
                UserOrder.PowerValue=50;
                UserOrder.direction=CCW;
            }

            // if in normal mode, rev key pressed, and speed is enough to lock regulator
            if(GetKeyCode()==KEY_REV && OrderLocked==FALSE && UserOrder.direction==CW && UserOrder.PowerValue>SPEED_REGULATOR_THRESHOLD)
            {
                OrderLocked=TRUE;
                UserOrder.PowerValue=ThrottlePosition;
                UserOrder.direction=CW;
            }
            

//            //------- check to see if REV button has been pressed after FRONT
//
//
//            if(GetKeyCode()== KEY_BOTH && OldKeyCode==KEY_FRONT && KeyEvent==FALSE)
//            {
//                KeyEvent=TRUE;
//            }
//
//            //------- scan for keys, and load the user order, according to keys
//
//            if(GetKeyCode()==KEY_NONE)
//            {
//                UserOrder.PowerValue=0;
//
//            }
//
//            if(GetKeyCode()==KEY_FRONT || GetKeyCode()==KEY_BOTH )
//            {
//                if(UserOrder.PowerValue!=CW_MAX_SPEED && UserOrder.PowerValue!=CW_MIN_SPEED  )
//                {
//                    UserOrder.PowerValue=CW_MAX_SPEED;
//                    UserOrder.direction=CW;
//
//                }
//
//                if(KeyEvent && UserOrder.PowerValue==CW_MAX_SPEED )
//                {
//                    UserOrder.PowerValue=CW_MIN_SPEED;
//                    UserOrder.direction=CW;
//                    KeyEvent=FALSE;
//                }
//                if(KeyEvent && UserOrder.PowerValue==CW_MIN_SPEED )
//                {
//                    UserOrder.PowerValue=CW_MAX_SPEED;
//                    UserOrder.direction=CW;
//                    KeyEvent=FALSE;
//                }
//            }
//
//            if(GetKeyCode()==KEY_REV)
//            {
//                UserOrder.PowerValue=70;
//                UserOrder.direction=CCW;
//
//            }
//
//
//            OldKeyCode=GetKeyCode();

        }
              


    }

}

