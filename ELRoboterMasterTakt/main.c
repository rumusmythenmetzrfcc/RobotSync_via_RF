/*
 * ELRoboterMasterTakt.c
 *
 * Created: 08.09.2016 09:23:56
 * Author : 1
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
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
	
	
	PRR0 = _BV(PRTIM1) | _BV(PRSPI) | _BV(PRADC);
	PRR1 = _BV(PRUSB) | _BV(PRTIM3) | _BV(PRUSART1);
	ACSR |= _BV(ACD);
}

volatile uint16_t milisecs;

ISR(TIMER0_OVF_vect){
	GPIOR1++;
	if(GPIOR1 > GPIOR2){
		GPIOR1 = 0;
		milisecs++;
	}
}

int 
main(void){
    uint8_t contentPE;
	enum{
	    ELSTOP,
	    ELFWD,
	    ELBWD,
	    ELTR,
	    ELTL
    } __attribute__((packed)) mode = ELSTOP;
    enum{
	    ELSTOPM,
	    ELFWDM,
	    ELBWDM,
	    ELTRM,
	    ELTLM
    } __attribute__((packed)) modememory = ELFWDM;

	iocon();
	
	(void)writePE(IOCON,IOCONV);
	(void)writePE(IODIRA,IODIRABV);
	(void)writePE(IODIRB,IODIRABV);
	
	PCIFR |= _BV(PCIF0);
	wait(&milisecs, MILISECSTOGO);
    
	for(;;) {
		wait(&milisecs, MILISECSTOGO);
		PORTB &= CLEARLEDS;
		
		switch(mode) {
			case ELSTOP:
			drive(0,0);
			mode = modememory;
			
			switch(modememory){
				case ELSTOPM:
				(void)writePE(OLATA,ELSTOPMOTO);
				modememory = ELFWDM;
				break;
				case ELBWDM:
				(void)writePE(OLATA,ELBACKWARD);
				break;
				case ELTRM:
				(void)writePE(OLATA,ELTURNRIGH);
				break;
				case ELTLM:
				(void)writePE(OLATA,ELTURNLEFT);
				break;
				case ELFWDM:
				(void)writePE(OLATA,ELFORWARDS);
			}
			
			break;
			case ELFWD:
			(void)writePE(OLATA,ELSTOPMOTO);
			drive(3,3);
			mode = ELSTOP;
			modememory = ELBWDM;
			break;
			case ELBWD:
			(void)writePE(OLATA,ELSTOPMOTO);
			drive(-3,-3);
			mode = ELSTOP;
			modememory = ELTRM;
			break;
			case ELTR:
			(void)writePE(OLATA,ELSTOPMOTO);
			drive(3,-3);
			mode = ELSTOP;
			modememory = ELTLM;
			break;
			case ELTL:
			(void)writePE(OLATA,ELSTOPMOTO);
			drive(-3,3);
			mode = ELSTOP;
			modememory = ELFWDM;
			break;
			default:
			(void)writePE(OLATA,ELERROR);
			drive(0,0);
		}
		
		SignalForOtherMCU();
		loop_until_bit_is_set(PCIFR,PCIF0);
		(void)readPE(&contentPE, GPIOA);
		
		switch(contentPE) {
			case RFALLOKAY:
			PORTB |= _BV(PORTB7);
			while(milisecs < (milisecs + 70));
			PORTB &= ~_BV(PORTB7);
			break;
			case RFLEDFRIG:
			PORTB |= _BV(LEDONE);
			break;
			case RFLEDBRIG:
			PORTB |= _BV(LEDTWO);
			break;
			case RFLEDFLEF:
			PORTB |= _BV(LEDTHREE);
			break;
			case RFLEDBLEF:
			PORTB |= _BV(LEDFOUR);
			break;
			default:
			drive(0,0);
			mode = ELSTOP;
			modememory = ELSTOPM;
			wait(&milisecs, ERRORWAITTIME);
		}
		
		PCIFR |= _BV(PCIF0);
    }
}

