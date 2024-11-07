

void PORT_init() {
	
	// low inits UART, high intis Wrooom
	DDRD	= 0xFA;	
	DDRB	= 0xFF;
	// inits PWM interrupt
	TCCR0A	= 0x03;
	TCCR0B	= 0x01;
}