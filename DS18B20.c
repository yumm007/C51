#include <STC12C5A60S2.H>
#include "lib.h"
#include "DS18B20.h"
#include "usart.h"
#include <stdio.h>

sbit D = P1 ^ 3;

#define CMD_READ_ID		0x33
#define CMD_SKIP_ID		0xCC
#define CMD_READ		0xBE
#define CMD_CONV		0x44

static bool reset_ds18b20(void) {
	bool ret;
	D= 1;
	delay_us(2);
	D = 0;
	delay_us(480);
	D = 1;
	delay_us(60);
	ret = !D;
	if (ret)
		delay_us(420);
	delay_us(480);

	return ret;
}

static u8 read_byte(void) {
	u8 ret, i;

    D = 1;
	for (i = 0, ret = 0; i < 8; i++) {
		D = 0;
		delay_us(2);
		D = 1;
		delay_us(8);
		if (D)
			ret |= (1 << i);
		delay_us(50);
	}
	//delay_us(60);
	return ret;
}

static void write_byte(u8 val) {
	u8 i;
	
	for (i = 0; i < 8; i++, val >>= 1) {
		D = 0;
		delay_us(12);
		D = val & 0x1;
		delay_us(60);
		D = 1;
	}	
	//delay_us(60);
	return;
}

void init_ds18b20(void) {
	;
}


float read_tmp(void) {

	u8 temp_h = 0, temp_l = 0;

	reset_ds18b20();
	write_byte(CMD_SKIP_ID);
	write_byte(CMD_CONV);

	delay_ms(1000);

 	reset_ds18b20();
	write_byte(CMD_SKIP_ID);
	write_byte(CMD_READ);

	temp_l = read_byte();
	temp_h = read_byte();

	return ((temp_h << 8) + temp_l) * 0.0625;
}
