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


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

// structure used to count 10ms tick
struct RTC_counter TickCounter;

//  variable for CAN  FIFO buffer and CAN adress
struct CANTxFifo CANTxFifo;
struct CANRxFifo CANRxFifo;
char LocalCanAdress;

// variable used for sound generation
struct SoundMsg SoundMsg;

// flag for sound timer tick
char SoundTimerTicked;

// variable  used for keyboard
struct KeybMsg KeybMsg;

// variable used for led display
struct LedDisp LedDisp;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
    char TempVar;
    unsigned int TempVarInt;

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
    ECANInitialize();
    //Set MASK and Filters for CAN
    ECANFiltersInit();

    // Timers configuration
    ConfigureTimers();

    // PWM outputs configuration
    ConfigureCCP();

    //----------------------------------------------------
    //----------  Global variables initialisation --------
    //----------------------------------------------------

     // initialize CAN tx FIFO
    CANTxFifoInit();
    CANRxFifoInit();

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
        //-------------         CAN command interpretor for received messages   ----------
        //--------------------------------------------------------------------------------

        //****  LCD message *****
       
        if (!CANRxFifo.FifoEmpty)
        {

            TempCANRxMsg = GetCANRxFifo();

            if (TempCANRxMsg.id == (CAN_MESSAGE_IHM_TYPE << 7 | LocalCanAdress << 4 | LCD_MSG_TYPE))
            {
                if (TempCANRxMsg.data_RX[0] == LCD_CLEAR_MSG_CODE && TempCANRxMsg.dataLen == LCD_CLEAR_MSG_LEN)
                {
                    
                }
                else if (TempCANRxMsg.data_RX[0] == LCD_SEND_CHAR_MSG_CODE && TempCANRxMsg.dataLen == LCD_SEND_CHAR_MSG_LEN)
                {
                   
                }
                else if (TempCANRxMsg.data_RX[0] == LCD_SEND_COMMAND_MSG_CODE && TempCANRxMsg.dataLen == LCD_SEND_COMMAND_MSG_LEN)
                {
                    
                }
            }

           
            //****  Software Version message *****

            else if (TempCANRxMsg.id == (CAN_MESSAGE_IHM_TYPE << 7 | LocalCanAdress << 4 | SOFT_VERSION_MESSAGE_ADRESS))
            {
                if (TempCANRxMsg.dataLen == SOFT_VERSION_MESSAGE_LEN && TempCANRxMsg.flags == ECAN_RX_RTR_FRAME)
                {
                    TempCANTxMsg.data_TX[0]=MAJOR_SW_VERSION;
                    TempCANTxMsg.data_TX[1]=MINOR_SW_VERSION;
                    TempCANTxMsg.dataLen= SOFT_VERSION_MESSAGE_LEN;
                    TempCANTxMsg.id = (CAN_MESSAGE_IHM_TYPE << 7 | LocalCanAdress <<4 | SOFT_VERSION_MESSAGE_ADRESS );
                    TempCANTxMsg.flags = ECAN_TX_STD_FRAME;
                    PutCANTxFifo(TempCANTxMsg);
                }
            }

            //****  Hardware Version message *****

            else if (TempCANRxMsg.id == (CAN_MESSAGE_IHM_TYPE << 7 | LocalCanAdress << 4 | BOARD_VERSION_MESSAGE_ADRESS))
            {
                
                if (TempCANRxMsg.dataLen == BOARD_VERSION_MESSAGE_LEN && TempCANRxMsg.flags == ECAN_RX_RTR_FRAME)
                {
                    TempCANTxMsg.data_TX[0]=BOARD_NUMBER;
                    TempCANTxMsg.data_TX[1]=BOARD_REVISION;
                    TempCANTxMsg.dataLen= BOARD_VERSION_MESSAGE_LEN;
                    TempCANTxMsg.id = (CAN_MESSAGE_IHM_TYPE << 7 | LocalCanAdress <<4 | BOARD_VERSION_MESSAGE_ADRESS );
                    TempCANTxMsg.flags = ECAN_TX_STD_FRAME;
                    PutCANTxFifo(TempCANTxMsg);
                }
            }
        }


        //--------------------------------------------------------------------------------
        // ---  Send can message if TXB0 buffer free, and data available in CAN TX FIFO --
        //--------------------------------------------------------------------------------
        
        if(!CANTxFifo.FifoEmpty && !TXB0CONbits.TXREQ)          // if fifo is not empty and buffer0 empty
        {
            TempCANTxMsg=GetCANTxFifo();
            ECANSendMessage(TempCANTxMsg.id,TempCANTxMsg.data_TX,TempCANTxMsg.dataLen,TempCANTxMsg.flags);  // fill tx buffer with Fifo data
        }


    }

}

