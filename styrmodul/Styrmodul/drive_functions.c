void north(void) {
	PORTD |= 0x30;
}

void south(void) {
	PORTD &= 0xCF;
}

void east(void) {
	PORTD &= 0xDF;
	PORTD |= 0x10;
}

void west(void) {
	PORTD &= 0xEF;
	PORTD |= 0x20;
}

uint8_t speed_div_1 (void) {
	return TCNT0;
}

uint8_t speed_div_8 (void) {
	return TCNT1;
}

uint8_t speed_div_64 (void) {
	return TCNT2;
}

uint8_t speed_div_256 (void) {
	return TCNT3;
}

uint8_t speed_select(const uint8_t speed) {
	switch (speed) {
		case 0:
			return TCNT0; // 50% on, 50 % off
		case 1:
			return TCNT0 & TCNT1; // 75% on, 25 % off
		case 2:
			return TCNT0 & TCNT1 & TCNT2; // 87,5% on, 12,5% off
		case 3:
			return TCNT0 & TCNT1 & TCNT2 & TCNT3; // 93,75% on, 6,25% off
		default:
			return TCNT0; // 50% on, 50 % off
	}
	return TCNT0; //FAILSAFE
}
