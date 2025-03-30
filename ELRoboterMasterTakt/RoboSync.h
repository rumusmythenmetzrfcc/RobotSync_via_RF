#ifndef __ROBOSYNCCODES_H__
#define __ROBOSYNCCODES_H__

#include <stdint.h>

//I2C Register Codes:
#define MCPSLA 0x4a
#define MAX_ITER 200
#define OLATA 0x14 //For writing
#define OLATB 0x15
#define GPIOA 0x12 //
#define GPIOB 0x13
//Main Configuration setup
#define IOCON 0x0a
//Configuration Value of IOCON
#define IOCONV 0x20 //Byte Mode
//Direction Setup
#define IODIRA 0x00
#define IODIRB 0x01
//Configuration Direction Register value for both Ports
#define IODIRABV 0x00//Outputs

//Robo State Codes:
#define ELSTOPMOTO 0x01
#define ELFORWARDS 0x02
#define ELBACKWARD 0x04
#define ELTURNRIGH 0x06
#define ELTURNLEFT 0x08
#define ELERROR 0x11
	//Additional State Messages:
#define RFBATEMPT 0x10//Battery of Slave Side is Empty
#define RFNEWSTAT 0x30//New Speed Statistic comes at next PE UPDATE
#define RFTIMEOUT 0x50//5min SleepMode Please
#define RFALLOKAY 0x21//Just say hello
#define RFLEDFRIG 0x31//LED Signaling (can be a 360° IR-LED with Sensor for estimating distance)
#define RFLEDBRIG 0x41//
#define RFLEDFLEF 0x71//
#define RFLEDBLEF 0x81//

//Time definitions:
#define TICKAMOUNT 62U
#define ERRORWAITTIME 10000U
#define MILISECSTOGO 1198U

#define F_CPU 8000000UL

//Hardware PIN Defines:
	//Port B
#define LEDONE 0U
#define LEDTWO 1U
#define LEDTHREE 2U
#define LEDFOUR 3U
#define MOTOR_R_EN 6U
#define BEEPER 7U
	//Port C
#define MOTOR_R_BWD 6U
	//Port D
#define LEDRED 2U
#define LEDGREEN 3U
#define MOTOR_L_EN 7U
	//Port E
#define SIGNALPRIMARYTRANSMITTER 6U
	//Port F
#define MOTOR_L_FWD 5U
#define MOTOR_L_BWD 6U
#define MOTOR_R_FWD 7U
	//Port B Masks
#define CLEARLEDS 0xf0
//Function prototypes:
void
iocon(void);

void
wait(volatile uint16_t*, uint16_t);

void
SignalForOtherMCU(void);

void
drive(uint8_t, uint8_t);

int8_t
writePE(uint8_t, uint8_t);

int8_t
readPE(uint8_t*, uint8_t);

#endif