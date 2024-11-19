void PORT_init(void) {
	// low inits UART, high intis Wrooom
	DDRD	= 0xFA;	
	DDRB	= 0xFF;  // INT2 is in use
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
	
	TCCR3B |= (1 << WGM32);
	TIMSK3 |= (1 << OCIE3A);
	//sei();
	OCR3A = 512;
	TCCR3B |= ((1 << CS30) | (1 << CS31));
	
	
	//TCNT3 = 0x00;
	//TCCR3A = 0x00; //Clear OCnA on Compare Match, clear OCnA at BOTTOM (inverting mode)
	//TCCR3A = (1 << WGM31)
	//TCCR3B = (1 << WGM32); //CTC

	//OCR3A = 128;

	//TIMSK3 = (1 << OCIE3A);


	/*
	TCCR3A = (1 << COM3A1) | (1 << WGM30);  // Non-inverted PWM on OC3A, Fast PWM mode
	TCCR3B = (1 << WGM32);// | (1 << CS31);    // Fast PWM mode, prescaler = 8
	TCCR3B |= 0x05;
	// Set the OCR3A value for the PWM duty cycle (example)
	OCR3A = 128;  // Set the compare match value for PWM (50% duty cycle for 8-bit)

	// Enable the interrupt for compare match A (OCR3A)
	TIMSK3 = (1 << OCIE3A);
	
	EICRA = (ISC21 << 1) | (ISC20 << 1);
	EIMSK |= (1 << INT2);
*/
}