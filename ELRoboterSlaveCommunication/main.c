/*
 * ELRoboterSlaveCommunication.c
 *
 * Created: 08.09.2016 19:40:29
 * Author : 1
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/sfr_defs.h>
#include "RoboSync.h"

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
main(void) {
	iocon();
	writePE(IOCON,IOCONV);
	writePE(IODIRA,IODIRABV);
	writePE(IODIRB,IODIRABV);
	write_register_NRF(SETUP_RETR,SETUP_RETRV);
	write_register_NRF(RF_CHPTX,RF_CHPTXV);
	write_register_NRF(RF_SETUP,RF_SETUPV);
	write_register_NRF(RX_PW_P0PTX,RX_PW_P0PTXV);
	write_register_NRF(FEATUREPTX,FEATUREPTXV);
	write_register_NRF(DYNPDPTX,DYNPDPTXV);
	write_register_NRF(CONFIGPTX,CONFIGPTXVPRX);
	
	PCIFR |= _BV(PCIF0);
	write_fifo_NRF(RFALLOKAY);
    PORTB |= _BV(SPI_CE);
	for(;;) {
		loop_until_bit_is_set(PCIFR,PCIF0);
		read_status_NRF();
		PORTB &= ~_BV(SPI_CE);
		
		switch (GPIOR2) {
			case STATUSPTXCLEARACKPAY:
			read_fifo_NRF();
			break;
			case STATUSPTXCLEAR_DR:
			read_fifo_NRF();
			break;
			default:
			GPIOR1 = ELERROR;
		}
		
		switch(GPIOR1){
			case ELSTOPMOTO:
			writePE(OLATA,ELSTOPMOTO);
			SignalForOtherMCU();
			write_fifo_NRF(RFALLOKAY);
			break;
			case ELFORWARDS:
			writePE(OLATA,ELFORWARDS);
			SignalForOtherMCU();
			write_fifo_NRF(RFLEDFRIG);
			break;
			case ELBACKWARD:
			writePE(OLATA,ELBACKWARD);
			SignalForOtherMCU();
			write_fifo_NRF(RFLEDBRIG);
			break;
			case ELTURNRIGH:
			writePE(OLATA,ELTURNRIGH);
			SignalForOtherMCU();
			write_fifo_NRF(RFLEDFLEF);
			break;
			case ELTURNLEFT:
			writePE(OLATA,ELTURNLEFT);
			SignalForOtherMCU();
			write_fifo_NRF(RFLEDBLEF);
			break;
			default:
			writePE(OLATA,ELERROR);
			SignalForOtherMCU();
			write_fifo_NRF(ELERROR);
		}
		
		PCIFR |= _BV(PCIF0);
		write_register_NRF(STATUSPTX,STATUSPTXCLEARALL);
		PORTB |= _BV(SPI_CE);
    }
}

