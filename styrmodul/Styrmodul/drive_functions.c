uint8_t speed_div_1 (void) { return TCNT0; }

uint8_t speed_div_8 (void) { return TCNT1; }

uint8_t speed_div_64 (void) { return TCNT2; }

uint8_t speed_div_256 (void) { return TCNT3; }

uint8_t speed_select(const uint8_t* speed) {
	switch (*speed) {
		case 0x00:
			return 0x00;
		case 0x01:
			return TCNT0; // 50% on, 50 % off
		case 0x02:
			return TCNT0 | TCNT1; // 75% on, 25 % off
		case 0x03:
			return TCNT0 | TCNT1 | TCNT2; // 87,5% on, 12,5% off
		case 0x04:
			return TCNT0 | TCNT1 | TCNT2 | TCNT3; // 93,75% on, 6,25% off
		default:
			return TCNT0; // 50% on, 50 % off
	}
	return TCNT0; //FAILSAFE
}

void north(const uint8_t* speed_left, const uint8_t* speed_right)
{
	PORTD = ( (speed_select(*speed_left) & 0x80) | ((speed_select(*speed_right) >> 1) & 0x40) ) | 0x30;
}

void south(const uint8_t* speed_left, const uint8_t* speed_right)
{
	PORTD = ( (speed_select(*speed_left) & 0x80) | ((speed_select(*speed_right) >> 1) & 0x40) ) | 0x00;
}

void west(const uint8_t* speed_left, const uint8_t* speed_right)
{
	PORTD = ( (speed_select(*speed_left) & 0x80) | ((speed_select(*speed_right) >> 1) & 0x40) ) | 0x20;
}

void east(const uint8_t* speed_left, const uint8_t* speed_right)
{
	PORTD = ( (speed_select(*speed_left) & 0x80) | ((speed_select(*speed_right) >> 1) & 0x40) ) | 0x10;
}

void stop()
{
	PORTD = 0x00;
}

/***********************************
	Drive Instruction Manual:
	N: Drive forward
	S: Reverse
	W: Turn left
	E: Turn right
	
	X: Stop
***********************************/

void drive(const uint8_t* drive_instr, const uint8_t* speed_left, const uint8_t* speed_right)
{
  cli();
  UART_Transmit_Instr_Received();
  switch(*drive_instr)
  {
    case 'N': north(*speed_left, *speed_right); break;
    case 'S': south(*speed_left, *speed_right); break;
    case 'W': west(*speed_left, *speed_right); break;
    case 'E': east(*speed_left, *speed_right); break;
    case 'X': stop(); break;
    default: stop(); break;
  }
  UART_Transmit_Instr_Done();
  sei();
}

void drive_test()
{
	cli();
	for (int j = 0; j < 30; ++j)
		for (int i = 0; i < 9999; ++i)
			north(1,1);
	
	for (int j = 0; j < 99; ++j)
		for (int i = 0; i < 9999; ++i)
			stop();
	
	for (int j = 0; j < 30; ++j)
		for (int i = 0; i < 9999; ++i)
			south(1,1);
	
	for (int j = 0; j < 99; ++j)
		for (int i = 0; i < 9999; ++i)
			stop();
	sei();
}

void drive_40_cm(const uint8_t speed_left, const uint8_t speed_right)
{
	uint8_t unit = 0;
	while (unit < 21)
	{
		north(speed_left,speed_right);
	}
	stop();
}