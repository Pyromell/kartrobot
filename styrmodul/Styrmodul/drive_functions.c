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
/*
1 full forward
void full forward() {
	PORTD |= 0x30;
}


2 slightly right
3 slightly left
4 hard right
5 hard left
*/