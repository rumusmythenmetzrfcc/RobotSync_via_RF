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

//SPI Register Codes:
	//SPI Quick Commands:
#define W_REGISTER 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xa0
#define FLUSH_TX 0xe1
#define NOOPERATION 0xff
	//SPI Registers and Values:
#define SETUP_RETR 0x04
#define SETUP_RETRV 0x2f
#define RF_CHPTX 0x05
#define RF_CHPTXV 0x64//2500GHz
#define RF_SETUP 0x06
#define RF_SETUPV 0x06
		
#define STATUSPTX 0x07
		//SPI Self created Status Codes:
#define STATUSPTXCLEARALL 0x70
#define STATUSPTXCLEARACKPAY 0x60
#define STATUSPTXCLEAR_DR 0x40
#define STATUSPTXCLEAR_DS 0x20
#define STATUSPTXCLEAR_RT 0x10

#define RX_PW_P0PTX 0x11
#define RX_PW_P0PTXV 0x01

#define FEATUREPTX 0x1d
#define FEATUREPTXV 0x06

#define DYNPDPTX 0x1c
#define DYNPDPTXV 0x01

#define CONFIGPTX 0x00
#define CONFIGPTXV 0x0a
#define CONFIGPTXVPRX 0x0b

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

//Hardware PIN Defines Robo:
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
//Hardware PIN Defines Extension PCB:
	//Port B
#define SPI_CSN 0U
#define SPI_CLK 1U
#define SPI_MOSI 2U
#define SPI_CE 4U
#define SPI_IRQ 5U
#define SIGNALOTHERMCU 6U

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

void
write_register_NRF(uint8_t, uint8_t);

void
write_fifo_NRF(uint8_t);

void
read_fifo_NRF(void);

void
read_status_NRF(void);

void
flush_fifotx_NRF(void);
#endif