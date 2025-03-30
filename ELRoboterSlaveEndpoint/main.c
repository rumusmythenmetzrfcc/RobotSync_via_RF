/*
 * ELRoboterSlaveEndpoint.c
 *
 * Created: 08.09.2016 20:10:13
 * Author : 1
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <avr/wdt.h>
#include "RoboSync.h"

void handle_mcucsr(void)
__attribute__((section(".init3")))
__attribute__((naked));
void handle_mcucsr(void) {
	MCUSR = 0;
	
	MCUCR |= _BV(JTD);
	MCUCR |= _BV(JTD);
	CLKPR = _BV(7);
	CLKPR = _BV(0);
	
	PRR0 = _BV(PRTIM1) | _BV(PRSPI) | _BV(PRADC);
	PRR1 = _BV(PRUSB) | _BV(PRTIM3) | _BV(PRUSART1);
	ACSR |= _BV(ACD);	
	
}

volatile uint16_t milisecs;

ISR(TIMER0_OVF_vect) {
	GPIOR1++;
	if(GPIOR1 > GPIOR2) {
		GPIOR1 = 0;
		milisecs++;
	}
}
ISR(WDT_vect) {
	drive(0,0);
	if(milisecs > WDTMILISECMX)
		milisecs = 0;
	wdt_reset();
}

int 
main(void) {
	uint8_t contentPE;
	iocon();
	PCIFR |= _BV(PCIF0);
    for(;;) {
		loop_until_bit_is_set(PCIFR,PCIF0);
		wdt_reset();
		readPE(&contentPE, GPIOA);
		milisecs = 0;
		PCIFR |= _BV(PCIF0);
		wait(&milisecs, MILISECSTOGOENDPOINT);
		
		switch(contentPE){
			case ELSTOPMOTO:
			drive(0,0);
			break;
			case ELFORWARDS:
			drive(3,3);
			break;
			case ELBACKWARD:
			drive(-3,-3);
			break;
			case ELTURNRIGH:
			drive(3,-3);
			break;
			case ELTURNLEFT:
			drive(-3,3);
			break;
			default:
			drive(0,0);
		}
    }
}

