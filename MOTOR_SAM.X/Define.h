/* 
 * File:   Define.h
 * Author: Bruno
 *
 * Created on 13 juin 2014, 16:13
 */

#ifndef DEFINE_H
#define	DEFINE_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* DEFINE_H */


#define MAJOR_SW_VERSION 1
#define MINOR_SW_VERSION 0

#define BOARD_NUMBER 3
#define BOARD_REVISION 1

#define _XTAL_FREQ 64000000

#define TMR0H_INIT  0xFB              // TMR0 value for 10ms tick
#define TMR0L_INIT  0x1E              // TMR0 value for 10ms tick

#define MOTOR_TICK_PERIOD 4             // IHM polling ticks each 10ms

//-------------------- define for CAN bus -------------

#define CANTX_FIFO_SIZE 10
#define CANRX_FIFO_SIZE 10

#define CAN_MESSAGE_MOTOR_TYPE 0x07

// motor order message
#define MOTOR_ORDER_TYPE 0x0
#define MOTOR_ORDER_LEN 2

// motor config message
#define MOTOR_CONFIG_TYPE 0x1
#define MOTOR_CONFIG_LEN 4

// motor info message
#define MOTOR_INFO_TYPE 0x2
#define MOTOR_INFO_LEN 3

// motor temperature message
#define MOTOR_TEMP_TYPE 0x3
#define MOTOR_TEMP_LEN 1

// motor odometry message
#define MOTOR_ODOM_TYPE 0x5
#define MOTOR_ODOM_LEN 2

#define SOFT_VERSION_MESSAGE_ADRESS 0x4
#define SOFT_VERSION_MESSAGE_LEN 2

#define BOARD_VERSION_MESSAGE_ADRESS 0xF
#define BOARD_VERSION_MESSAGE_LEN 2


//-------------------- define for motor control -------------

#define MEAN_TABLE_SIZE 64

#define CCP_PWM_MODE 0x0F
#define CW 1
#define CCW 0
#define MOTOR_OFF 2

#define PH1_H LATDbits.LATD3
#define PH2_H LATDbits.LATD4
#define PH1_L LATCbits.LATC2
#define PH2_L LATCbits.LATC6

#define REVERSE_TH 10

#define CW_MIN_SPEED 70
#define CW_MAX_SPEED 100

//------------------- define for key codes ------------------

#define KEY_NONE 0b00000000
#define KEY_FRONT 0b00010000
#define KEY_REV 0b00100000
#define KEY_BOTH 0b00110000

#define THROTTLE_OFFSET 0x297
#define THROTTLE_MAX_VALUE 0x8DA
#define THROTTLE_DIVIDER 18

#define SPEED_REGULATOR_THRESHOLD 25

#define TRUE    1
#define FALSE   0


