/*
 * main.c
 *
 *  Created on: 2013.07.25
 *      Author: Mateusz Plociennik
 */
#include <stdint.h>

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Pinout of ATtiny13A:
 *            ---\__/---
 *     RESET -|1      8|- VCC
 * (SWI) PB3 -|2      7|- PB2 (RLY)
 * (RES) PB4 -|3      6|- PB1 (TRG)
 *       GND -|4      5|- PB0 (LED)
 *            ----------
 */
#define LED	PB0	// Light-emitting Diode
#define RES	PB4	// Reserved (not connected)
#define RLY	PB2	// Relay Control
#define SWI	PB3	// User Switch
#define TRG	PB1	// Trigger Signal

#define ledOn()				(PORTB |= (1 << LED))
#define ledOff()			(PORTB &= ~(1 << LED))

#define isSwitchReleased()	(PINB & (1 << SWI))

#define relayOn()			(PORTB |= (1 << RLY))
#define relayOff()			(PORTB &= ~(1 << RLY))

#define isTriggerHigh()		(PINB & (1 << TRG))

#define PROG_EN_TIME 	9 // cycles
#define RLY_OPR_TIME 	7 // ms
#define SWITCH_DELAY 	10 // ms

// Cycle time = 435.2 ms
#define LED_ON_TIME		10 // ms
#define LED_OFF_TIME	10 // ms
#define LED_PROG_TIME	20 // ms

enum mode_e
{
	MODE_WAIT 		= 0x01,
	MODE_DELAY 		= 0x02,
	MODE_LATCH		= 0x03,
	MODE_PROG_WAIT 	= 0x11,
	MODE_PROG_DELAY	= 0x12,
	MODE_PROG_LATCH	= 0x13,
};

#define MODE_PROG_MASK	0xF0
#define MODE_ENUM_MASK	0x0F

volatile uint8_t mode = 0;
volatile uint8_t time = 0;

uint8_t EEMEM ee_delay_time = 91;
volatile uint8_t delay_time;
uint8_t EEMEM ee_latch_time = 11;
volatile uint8_t latch_time;
uint8_t EEMEM ee_trigg_pol = 0;
volatile uint8_t trigg_pol;

int main()
{
	// Read EEPROM
	delay_time = eeprom_read_byte(&ee_delay_time);
	latch_time = eeprom_read_byte(&ee_latch_time);
	trigg_pol = eeprom_read_byte(&ee_trigg_pol);

	// Port Pin Configurations
	DDRB = (1 << LED) | (0 << RES) | (1 << RLY) | (0 << SWI) | (0 << TRG);
	PORTB = (0 << LED) | (1 << RES) | (0 << RLY) | (1 << SWI) | (0 << TRG);
	MCUCR = (1 << ISC01) | (trigg_pol ? 1 : 0 << ISC00);

	mode = MODE_WAIT;

	// PWM Phase Correct, clkIO/1024, enable interrupt
	TCCR0A = (0 << WGM01) | (1 << WGM00);
	TCCR0B = (0 << WGM02) | (1 << CS02) | (0 << CS01) | (1 << CS00);

	GIMSK = (1 << INT0); // Enable INT0 interrupt
	TIMSK0 = (1 << TOIE0); // Enable TIM0_OVF interrupt

	sei();

	while (1) {} // infinite loop
}

ISR(INT0_vect)
{
	_delay_ms(SWITCH_DELAY);

	switch (mode)
	{
	case MODE_PROG_WAIT:
		trigg_pol = isTriggerHigh() ? 1 : 0;
		eeprom_update_byte(&ee_trigg_pol, trigg_pol);

		// The falling / rising edge of INT0 generates an interrupt request
		MCUCR = (1 << ISC01) | (trigg_pol ? 1 : 0 << ISC00);

		/* no break */
	case MODE_WAIT:
		++mode;

		TCNT0 = 0;
		time = 0;

		break;
	default:
		break;
	}
}

ISR(TIM0_OVF_vect)
{
	++time;

	switch (mode)
	{
	case MODE_WAIT:
		if (isSwitchReleased())
		{
			time = 0;
		}
		else if (time == PROG_EN_TIME)
		{
			mode = MODE_PROG_WAIT;

			// Any logical change on INT0 generates an interrupt request
			MCUCR = (0 << ISC01) | (1 << ISC00);
		}
		break;
	case MODE_DELAY:
		if (time == delay_time)
		{
			++mode;

			relayOn();
			time = 0;
		}
		break;
	case MODE_LATCH:
		if (time == latch_time)
		{
			mode = MODE_WAIT;

			relayOff();
			time = 0;
		}
		break;
	case MODE_PROG_WAIT:
		break;
	case MODE_PROG_DELAY:
		if (!isSwitchReleased())
		{
			++mode;

			delay_time = time;
			eeprom_update_byte(&ee_delay_time, delay_time);

			relayOn();
			time = 0;
		}
		break;
	case MODE_PROG_LATCH:
		if (isSwitchReleased())
		{
			mode = MODE_WAIT;

			latch_time = time;
			eeprom_update_byte(&ee_latch_time, latch_time);

			relayOff();
			time = 0;
		}
		break;
	default:
		{
			mode = MODE_WAIT;
		}
	}

	uint8_t i;
	for(i = 0; i < (mode & MODE_ENUM_MASK); ++i)
	{
		ledOn();
		if (mode & MODE_PROG_MASK) // Programming mode
		{
			_delay_ms(LED_PROG_TIME);
		}
		else // Normal mode
		{
			_delay_ms(LED_ON_TIME);
		}
		ledOff();
		_delay_ms(LED_OFF_TIME);
	}
}

