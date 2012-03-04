#include "lib.h"
#include <STC12C5A60S2.H>

void delay_init(void) {

	TMOD &= 0xf0;
	TMOD |= BIT0; //定时器0为16位定时器，

	AUXR |= BIT7;	  //T0x12 = 1 1T模式，即每个时钟周期对应一个机器周期，每个机器周期计时加1.
	
	ET0 = 1;
	EA  = 1;
}

#define T01MS	(0xffff - FOSC / 1000)
#define T01US	(0xffff - FOSC / 1000 /1000)

static u8 tm0_running = 0;
void tm0_isr(void) interrupt 1 {
	tm0_running = 0;	
} 

void delay_ms(unsigned int n) {

	while (n--) {
		//1毫秒的时间
		TL0 = T01MS;
		TH0 = (T01MS >> 8);

		tm0_running = 1;
		
		TR0 = 1;	//启动定时器

		while (tm0_running)
			;
	}
	
	TR0 = 0;	
}

void delay_us(unsigned int n) {	
	//一个while循环6条指令，差不多每条指令需要2.5个时钟周期, 即每循环一次1us
	while (n--) {
		;
	}
}

