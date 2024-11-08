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
	*/
	
	TCCR0A	= 0x03; // Fast PWM, From 0x00 -> 0xFF
	TCCR0B	= 0x01;
	
	TCCR1A	= 0x03; // Fast PWM, From 0x00 -> 0xFF
	TCCR1B	= 0x02;
	
	TCCR2A	= 0x03; // Fast PWM, From 0x00 -> 0xFF
	TCCR2B	= 0x03;
	
	TCCR3A	= 0x03; // Fast PWM, From 0x00 -> 0xFF
	TCCR3B	= 0x04;
}