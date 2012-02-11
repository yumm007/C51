#include "lib.h"

void delay_ms(unsigned int n) {
	unsigned int i;

	while (n--)
		for (i = 0; i < 600; i++)
			;
}

void delay_us(unsigned int n) {
	//n *= 12 / 2;
	while (n--) {
		;
	}
}