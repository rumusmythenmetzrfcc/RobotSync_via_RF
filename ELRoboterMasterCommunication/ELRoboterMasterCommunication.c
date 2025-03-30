#include <avr/io.h>
#include <stdint.h>
#include <avr/sfr_defs.h>
#include "RoboSync.h"
#include <util/twi.h>

void
iocon(void) {
	DDRB = _BV(SIGNALOTHERMCU) | _BV(SPI_CE) | _BV(SPI_MOSI) | _BV(SPI_CLK) | _BV(SPI_CSN);
	PORTB = _BV(SPI_IRQ) | _BV(SPI_CSN);
	
	PCMSK0 = _BV(PCINT5) | _BV(PCINT7);

	TWSR = 0;
	TWBR = (F_CPU/100000UL - 16U)/2U;
	
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
	SPSR = _BV(SPI2X);
}
void
inline
SignalForOtherMCU(void) {
	PORTB ^= _BV(SIGNALOTHERMCU);
}
int8_t
inline
writePE(uint8_t eeaddr, uint8_t buf) {
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

		case TW_MT_ARB_LOST:
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
	loop_until_bit_is_set(TWCR,TWINT);/* wait for transmission */
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
	
	TWDR = buf;
	TWCR = _BV(TWINT) | _BV(TWEN); /* start transmission */
	loop_until_bit_is_set(TWCR,TWINT);; /* wait for transmission */
	switch ((GPIOR0 = TW_STATUS)){
		case TW_MT_DATA_NACK:
		goto error;		/* device write protected -- Note [16] */

		case TW_MT_DATA_ACK:
		rv++;
		break;
		
		default:
		goto error;
	}
	quit:
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

	return rv;

	error:
	rv = -1;
	goto quit;
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
void
inline
write_register_NRF(uint8_t address, uint8_t value) {
	PORTB &= ~_BV(SPI_CSN);
	
	address = (address | W_REGISTER);
	SPDR = address;
	loop_until_bit_is_set(SPSR,SPIF);
	
	GPIOR2 = SPDR;
	SPDR = value;
	loop_until_bit_is_set(SPSR,SPIF);
	
	PORTB |= _BV(SPI_CSN);
}
void
inline
write_fifo_NRF(uint8_t payload) {
	PORTB &= ~_BV(SPI_CSN);
	
	SPDR = W_TX_PAYLOAD;
	loop_until_bit_is_set(SPSR,SPIF);
	GPIOR2 = SPDR;
	
	SPDR = payload;
	loop_until_bit_is_set(SPSR,SPIF);

	PORTB |= _BV(SPI_CSN);
}
void
inline
read_fifo_NRF(void) {
	PORTB &= ~_BV(SPI_CSN);

	SPDR = R_RX_PAYLOAD;
	loop_until_bit_is_set(SPSR,SPIF);
	GPIOR2 = SPDR;
	
	SPDR = 0x00;
	loop_until_bit_is_set(SPSR,SPIF);
	GPIOR1 = SPDR;

	PORTB |= _BV(SPI_CSN);
}
void
inline
read_status_NRF(void) {
	PORTB &= ~_BV(SPI_CSN);

	SPDR = NOOPERATION;
	loop_until_bit_is_set(SPSR,SPIF);
	GPIOR2 = SPDR;

	PORTB |= _BV(SPI_CSN);
}
void
inline
flush_fifotx_NRF(void){
	PORTB &= ~_BV(SPI_CSN);

	SPDR = FLUSH_TX;
	loop_until_bit_is_set(SPSR,SPIF);
	GPIOR2 = SPDR;

	PORTB |= _BV(SPI_CSN);
}