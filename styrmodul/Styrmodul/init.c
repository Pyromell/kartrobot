/***********************************
File Description:
	This file Handles the init of stuff

Pin Description:
	Pin 18 DIR1 (Direction for left track)
	Pin 19 DIR2 (Direction for right track)
	Pin 20 PWM1 ("Speed" for left track)
	Pin 21 PWM2 ("Speed" for right track)
	
TEMP ports:
	DDRD	= 0xFA; // NEEDED FOR DIR, not pwm (Pin 14-21)
	DDRB	= 0xFF; // used for error checking (Pin 1 - 8)
  
***********************************/

void PORT_Init(void) {
	DDRD = 0xFA; // low for UART, high for PWM
	DDRB = 0xFF; // Only used for error checking
}

void PWM_Init(void) {
	// inits PWM interrupt
	/*
	0: clk I/O /1  (No prescaling)
	1: clk I/O /8  (From prescaler)
	2: clk I/O /64 (From prescaler)
	
	All PWM counters are from 0x00 -> 0xFF
	*/
	
	TCCR0A	= 0x03;
	TCCR0B	= 0x01;
	
	TCCR1A	= 0x03;
	TCCR1B	= 0x02;
	
	TCCR2A	= 0x03;
	TCCR2B	= 0x03;
	
	/*
	Timer Interrupt for control_sys
	Description for interrupt:
		The interrupt is triggered every 10ms (actually 9.98ms), with some exceptions.
		If an exception occurs (like another interrupt is active), then the
		next interrupt is triggered on the next 10ms, thus effectively skipping
		one interrupt.
		
	Setup for interrupt timer:
		Waveform Generation Mode = Reserved (WGM32)
		Clock prescale = clk I/O clk/8 (CS31)
		Interrupt vector = TIMER3_COMPA_vect (OCIE3A)
		Output Compare Register A = 20400 (OCR3A)
	*/
	TCCR3B |= (1 << WGM32);
	TCCR3B |= (1 << CS31);
	
	TIMSK3 |= (1 << OCIE3A);
	
	OCR3A = 20400; // this determines the time between interrupts
}

void Interrupt_Init()
{
	sei();
}