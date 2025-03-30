#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/twi.h>
#include <avr/wdt.h>
#include "RoboSync.h"
#include <util/delay.h>

void
iocon(void) {
	DDRB = _BV(LEDONE) | _BV(LEDTWO) | _BV(LEDTHREE) | _BV(LEDFOUR) | _BV(MOTOR_R_EN) | _BV(BEEPER);
	DDRC = _BV(MOTOR_R_BWD);
	DDRD = _BV(LEDRED) | _BV(LEDGREEN) | _BV(MOTOR_L_EN);
	DDRE = _BV(SIGNALTOMCU);
	DDRF = _BV(MOTOR_L_FWD) | _BV(MOTOR_L_BWD) | _BV(MOTOR_R_FWD);//former MOT_L_F, MOT_L_B, MOT_R_F

	GPIOR1 = 0;
	GPIOR2 = TICKAMOUNT;
	
	PLLCSR |= _BV(PINDIV);
	PLLCSR |= _BV(PLLE);
	_delay_ms(100);
	loop_until_bit_is_set(PLLCSR,PLOCK);
	PLLFRQ |= _BV(PLLTM0);
	
	TIMSK0 = _BV(TOIE0);
	TCCR0B = _BV(CS00);
	
	TCCR4A = _BV(PWM4B);
	TCCR4C = _BV(COM4B1S) | _BV(COM4D1) | _BV(PWM4D);
	TCCR4D = _BV(WGM40);
	TCCR4B = _BV(CS41);

	TCCR4E = _BV(TLOCK4);
	TC4H = 0x03;
	OCR4C = 0xFF;
	TC4H = (((uint16_t)SPEEDLEFT) >> 8);
	OCR4B = (uint8_t)(SPEEDLEFT);
	OCR4D = (uint8_t)(SPEEDRIGHT);
	TCCR4E &= ~_BV(TLOCK4);
	
	wdt_reset();
	WDTCSR |= _BV(WDCE) | _BV(WDE);
	WDTCSR  = _BV(WDIE) | _BV(WDP3);
	
	PCMSK0 = _BV(PCINT5);

	TWSR = 0;
	TWBR = (F_CPU/100000UL - 16U)/2U;
	
	sei();
}
void
inline
wait(volatile uint16_t* milisecs, uint16_t waittime) {
	while(*milisecs < waittime);
	*milisecs = 0;
}
void
inline
drive(uint8_t left, uint8_t right) {
	//left:
	if(left > 0){
		//Activate Forward
		PORTD &= ~_BV(MOTOR_L_EN);
		PORTF |= _BV(MOTOR_L_FWD);
		PORTF &= ~_BV(MOTOR_L_BWD);
	}
	else if(left < 0){
		//Activate Backward
		PORTD &= ~_BV(MOTOR_L_EN);
		PORTF &= ~_BV(MOTOR_L_FWD);
		PORTF |= _BV(MOTOR_L_BWD);
	}
	else{
		//Stop
		PORTD |= _BV(MOTOR_L_EN);
		PORTF &= ~(_BV(MOTOR_L_FWD) | _BV(MOTOR_L_BWD));
	}
	//right:
	if(right > 0){
		//Activate Forward
		PORTB &= ~_BV(MOTOR_R_EN);
		PORTF |= _BV(MOTOR_R_FWD);
		PORTC &= ~_BV(PORTC6);
	}
	else if(right < 0){
		//Activate Backward
		PORTB &= ~_BV(MOTOR_R_EN);
		PORTF &= ~_BV(MOTOR_R_FWD);
		PORTC |= _BV(MOTOR_R_BWD);
	}
	else{
		//Stop
		PORTB |= _BV(MOTOR_R_EN);
		PORTF &= ~_BV(MOTOR_R_FWD);
		PORTC &= ~_BV(MOTOR_R_BWD);
	}
}
int8_t
inline
readPE(uint8_t* contentPE, uint8_t eeaddr) {
	uint8_t sla, n = 0;
	volatile int8_t rv = 0;

	sla = MCPSLA;
	restart:
	if (n++ >= MAX_ITER)
	return -1;
	
	begin:
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
	loop_until_bit_is_set(TWCR,TWINT); /* wait for transmission */
	switch ((GPIOR0 = TW_STATUS)){
		case TW_REP_START:		/* OK, but should not happen */
		case TW_START:
		break;

		case TW_MT_ARB_LOST:	/* Note [9] */
		goto begin;

		default:
		return -1;		/* error: not in start condition */
		/* NB: do /not/ send stop condition */
	}
	
	TWDR = sla | TW_WRITE;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	loop_until_bit_is_set(TWCR,TWINT); /* wait for transmission */
	switch ((GPIOR0 = TW_STATUS)){
		case TW_MT_SLA_ACK:
		break;

		case TW_MT_SLA_NACK:	/* nack during select: device busy writing */
		goto restart;

		case TW_MT_ARB_LOST:	/* re-arbitrate */
		goto begin;

		default:
		goto error;		/* must send stop condition */
	}
	
	TWDR = eeaddr;		/* low 8 bits of addr */
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	loop_until_bit_is_set(TWCR,TWINT); /* wait for transmission */
	switch ((GPIOR0 = TW_STATUS)){
		case TW_MT_DATA_ACK:
		break;

		case TW_MT_DATA_NACK:
		goto quit;

		case TW_MT_ARB_LOST:
		goto begin;

		default:
		goto error;		/* must send stop condition */
	}
	
	
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send (rep.) start condition */
	loop_until_bit_is_set(TWCR,TWINT); /* wait for transmission */
	switch ((GPIOR0 = TW_STATUS)){
		case TW_START:		/* OK, but should not happen */
		case TW_REP_START:
		break;

		case TW_MT_ARB_LOST:
		goto begin;

		default:
		goto error;
	}
	
	TWDR = sla | TW_READ;
	TWCR = _BV(TWINT) | _BV(TWEN);
	loop_until_bit_is_set(TWCR,TWINT);
	switch ((GPIOR0 = TW_STATUS))
	{
		case TW_MR_SLA_ACK:
		break;

		case TW_MR_SLA_NACK:
		goto quit;

		case TW_MR_ARB_LOST:
		goto begin;

		default:
		goto error;
	}
	
	TWCR = _BV(TWINT) | _BV(TWEN);
	loop_until_bit_is_set(TWCR,TWINT);
	switch ((GPIOR0 = TW_STATUS)){
		case TW_MR_DATA_NACK:
		*contentPE = TWDR;
		rv++;
		break;
		
		case TW_MR_DATA_ACK:
		goto error;

		default:
		goto error;
	}
	
	quit:
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	return rv;

	error:
	rv = -1;
	goto quit;
}
