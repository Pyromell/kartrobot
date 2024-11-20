void PORT_init(void) {
	// low inits UART, high intis Wrooom
	DDRD	= 0xFA;	
	DDRB	= 0xFF; 
}
void PWM_init(void) {
	// inits PWM interrupt
	/*
	0: clk I/O /(No prescaling)
	1: clk I/O /8 (From prescaler)
	2: clk I/O /64 (From prescaler)
	3: clk I/O /256 (From prescaler)
	
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
	
	OCR3A = 20400;
}