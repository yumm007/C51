#include "lib.h"

void delay_ms(unsigned int n) {
	unsigned int i;

	while (n--)
		for (i = 0; i < 600; i++)
			;
}

void delay_us(unsigned int n) {
	//n *=4;
	while (n--) {
		_nop_();
		_nop_();
	}
}