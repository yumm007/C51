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
	delay_us(420);

	return ret;
}

void init_ds18b20(void) {
	printf("init ds18b20 %s.\n", reset_ds18b20() ? "ok" : "failed");
	
}


float read_tmp(void) {

	return 0.0;
}
