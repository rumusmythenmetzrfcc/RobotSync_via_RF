/*
 * ELRoboterMasterCommunication.c
 *
 * Created: 08.09.2016 18:27:57
 * Author : 1
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/sfr_defs.h>
#include "RoboSync.h"
#include <util/twi.h>

void handle_mcucsr(void)
__attribute__((section(".init3")))
__attribute__((naked));

void handle_mcucsr(void){
	MCUSR = 0;
	
	MCUCR |= _BV(JTD);
	MCUCR |= _BV(JTD);
	CLKPR = _BV(7);
	CLKPR = _BV(0);
	
	
	PRR0 = _BV(PRTIM1) | _BV(PRADC);
	PRR1 = _BV(PRUSB) | _BV(PRTIM3) | _BV(PRUSART1);
	ACSR |= _BV(ACD);
}

int 
main(void){
	uint8_t contentPE;
	
    iocon();
	write_register_NRF(SETUP_RETR,SETUP_RETRV);
	write_register_NRF(RF_CHPTX,RF_CHPTXV);
	write_register_NRF(RF_SETUP,RF_SETUPV);
	write_register_NRF(RX_PW_P0PTX,RX_PW_P0PTXV);
	write_register_NRF(FEATUREPTX,FEATUREPTXV);
	write_register_NRF(DYNPDPTX,DYNPDPTXV);
	write_register_NRF(CONFIGPTX,CONFIGPTXV);
	
	PCIFR |= _BV(PCIF0);
	
    for(;;) {
		loop_until_bit_is_set(PCIFR,PCIF0);
		PCIFR |= _BV(PCIF0);
		readPE(&contentPE, GPIOA);
	
		switch(contentPE) {
			case ELSTOPMOTO:
			write_fifo_NRF(ELSTOPMOTO);
			break;
			case ELFORWARDS:
			write_fifo_NRF(ELFORWARDS);
			break;
			case ELBACKWARD:
			write_fifo_NRF(ELBACKWARD);
			break;
			case ELTURNRIGH:
			write_fifo_NRF(ELTURNRIGH);
			break;
			case ELTURNLEFT:
			write_fifo_NRF(ELTURNLEFT);
			break;
			default:
			write_fifo_NRF(ELSTOPMOTO);
		}
	
		PORTB |= _BV(SPI_CE);
	
		loop_until_bit_is_set(PCIFR,PCIF0);
	
		PORTB &= ~_BV(SPI_CE);
		read_status_NRF();
	
		switch(GPIOR2) {
			case STATUSPTXCLEARACKPAY:
			read_fifo_NRF();
			break;
			case STATUSPTXCLEAR_DS:
			GPIOR1 = RFALLOKAY;
			break;
			default:
			GPIOR1 = ELERROR;
			flush_fifotx_NRF();
		}
	
		switch(GPIOR1) {
			case RFALLOKAY:
			writePE(OLATA,RFALLOKAY);
			break;
			case RFLEDFRIG:
			writePE(OLATA,RFLEDFRIG);
			break;
			case RFLEDBRIG:
			writePE(OLATA,RFLEDBRIG);
			break;
			case RFLEDFLEF:
			writePE(OLATA,RFLEDFLEF);
			break;
			case RFLEDBLEF:
			writePE(OLATA,RFLEDBLEF);
			break;
			default:
			writePE(OLATA,ELERROR);
		}
	
		SignalForOtherMCU();
		write_register_NRF(STATUSPTX,STATUSPTXCLEARALL);
		PCIFR |= _BV(PCIF0);
	}
}

