


void north() {
	PORTD |= 0x30;
}

void south() {
	PORTD &= 0xCF;
}

void east() {
	PORTD &= 0xDF;
	PORTD |= 0x10;
}

void west() {
	PORTD &= 0xEF;
	PORTD |= 0x20;
}
