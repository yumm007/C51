void delay_ms(unsigned int n) {
	unsigned int i;

	while (n--)
		for (i = 0; i < 600; i++)
			;
}