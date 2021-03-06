/*
 * RGBDriver.cpp
 *
 *  Created on: Mar 26, 2013
 *      Author: Chad Juliano
 */

#include "RGBDriver.h"
#include <Arduino.h>

RGBDriver g_rgbDriver = RGBDriver();

ISR (TIMER2_OVF_vect)
{
	g_rgbDriver.TimerIteration();
}

uint8_t const RGBDriver::_ledMap[] = {
		_BV(RR_SEL_LED0),
		_BV(RR_SEL_LED1),
		_BV(RR_SEL_LED2),
		_BV(RR_SEL_LED3),
		_BV(RR_SEL_LED4),
		_BV(RR_SEL_LED5)
};

RGBDriver::RGBDriver()
{
	SetLedAll(RGBColor::WHITE, 0);
	//Setup();
}

void RGBDriver::Setup()
{
	SetupLeds();

	// setup buttons
	pinMode(RR_BUTTON_1, INPUT);
	pinMode(RR_BUTTON_2, INPUT);
	digitalWrite(RR_BUTTON_1, HIGH);
	digitalWrite(RR_BUTTON_2, HIGH);

	SetupTimer();
	EnableTimer();
}

void RGBDriver::SetupLeds()
{
	// Set LED selector data directions
	RR_SEL_DDR |= RR_SEL_ALLLED;

	// Set colorport Data directions.
	RR_CPA_DDR |= RR_CPA_WHITE;
	RR_CPB_DDR |= RR_CPB_WHITE;

	SetLedAllOff();

	// set LED1 to green.
	//RR_SEL_PORT |= _BV(RR_SEL_LED1);
	//RR_CPB_PORT |= _BV(RR_CPB_GREEN);
}

void RGBDriver::SetLedAll(RGBColor::color_t color, uint8_t n_level)
{
	for(int n_led = 0; n_led < _LED_COUNT; n_led++)
	{
		_ledColors[n_led].SetColor(color, n_level);
	}
}

void RGBDriver::SetLedAll(const RGBColor& color)
{
	for(int n_led = 0; n_led < _LED_COUNT; n_led++)
	{
		_ledColors[n_led] = color;
	}
}

RGBColor& RGBDriver::operator[](uint8_t n_led)
{
	return _ledColors[n_led];
}

void RGBDriver::SetupTimer()
{
	// TCCR2A � Timer/counter control register A
	// COM2A1 = COM2A0 = 0: Normal port operation, OC2A disconnected
	TCCR2A = 0;

	// TCCR2B � Timer/counter control register B
	// WGM22 = 0: Normal port operation, OC0A disconnected
	// WGM22 = 1: Toggle OC2A on compare match
	TCCR2B &= ~_BV(WGM22);

	// Arduino runs at 16 Mhz...
	// Timer2 (8bit) Settings:
	// prescaler (frequency divider) values:   CS12    CS11   CS10
	//                                           0       0      0    stopped
	//                                           0       0      1      /1
	//                                           0       1      0      /8
	//                                           0       1      1      /32
	//                                           1       0      0      /64
	//                                           1       0      1      /128
	//                                           1       1      0      /256
	//                                           1       1      1      /1024
	// Timer/counter control register B (set to 1024)
	TCCR2B |= _BV(CS22) | _BV(CS20) | _BV(CS21);

	// enable global interrupts flag
	sei();
}

void RGBDriver::EnableTimer()
{
	// TCNT2 � Timer/counter register
	TCNT2 = _TIMER1_MAX - _TIMER1_CNT;

	// TIMSK2 � Timer/Counter2 interrupt mask register
	// TOIE2: Timer/Counter2 overflow interrupt enable
	TIMSK2 |= _BV(TOIE2);
}

void RGBDriver::DisableTimer(void)
{
	// turn off all LED's
	RR_SEL_PORT &= ~RR_SEL_ALLLED;

	// disable timer overflow
	// TIMSK2 � Timer/Counter2 interrupt mask register
	// TOIE2: Timer/Counter2 overflow interrupt enable
	TIMSK2 &= ~_BV(TOIE2);
}

void RGBDriver::SetLedAllOff()
{
	// pins LOW --> anodes LOW --> LEDs off
	RR_CPA_PORT &= ~RR_CPA_WHITE;
	RR_CPB_PORT &= ~RR_CPB_WHITE;

	// all pins HIGH --> cathodes HIGH --> LEDs off
	RR_SEL_PORT &= ~RR_SEL_ALLLED;
}

void RGBDriver::SetLedPorts(uint8_t n_selector, uint8_t cport_a, uint8_t cport_b)
{
	// clear all ports
	SetLedAllOff();

	// give ports a little time to react
	asm("nop");
	asm("nop");

	// select the appropriate LED
	RR_SEL_PORT = (RR_SEL_PORT & ~RR_SEL_ALLLED) | _ledMap[n_selector];

	// set colorports
	RR_CPA_PORT = (RR_CPA_PORT & ~RR_CPA_WHITE) | cport_a;
	RR_CPB_PORT = (RR_CPB_PORT & ~RR_CPB_WHITE) | cport_b;
}

void RGBDriver::TimerIteration()
{
	// reset timer
	EnableTimer();

	// we scale the _BRIGHT_MAX by a factor given by _SCALE_DIVISOR
	// [0],[1],[2],[3] => [0,1,2,3],[4,5,6,7],[8,9,10,11],[12,13,14,15]
	for (uint8_t n_level = _SCALE_DIVISOR - 1;
			n_level < RGBColor::_BRIGHT_MAX;
			n_level += _SCALE_DIVISOR)
	{
		uint8_t n_led = 0;

		for (uint8_t n_led_sel = 0; n_led_sel < 6; n_led_sel++)
		{
			uint8_t cport_a = 0;
			uint8_t cport_b = 0;

			RGBColor& ledColorA = _ledColors[n_led];
			n_led++;
			if (n_level < ledColorA._green) cport_a |= _BV(RR_CPA_GREEN);
			if (n_level < ledColorA._blue) cport_a |= _BV(RR_CPA_BLUE);
			if (n_level < ledColorA._red) cport_a |= _BV(RR_CPA_RED);

			RGBColor& ledColorB = _ledColors[n_led];
			n_led++;
			if (n_level < ledColorB._green) cport_b |= _BV(RR_CPB_GREEN);
			if (n_level < ledColorB._blue) cport_b |= _BV(RR_CPB_BLUE);
			if (n_level < ledColorB._red) cport_b |= _BV(RR_CPB_RED);

			SetLedPorts(n_led_sel, cport_a, cport_b);
		}
	}
	SetLedAllOff();
}
