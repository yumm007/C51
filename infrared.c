#include "infrared.h"
#include <STC12C5A60S2.H>
#include "lib.h"
#include "i2c.h"
#include "usart.h"
#include "string.h"
#include "spi.h"
#include "led.h"
#include "nrf24l01.h"
#include "zx12864.h"
#include <intrins.h>
#include <stdio.h>

enum {
	CHS0 = BIT0,
	CHS1 = BIT1,
	CHS2 = BIT2,
	ADC_START = BIT3,
	ADC_FLAG = BIT4,
	SPEED0 = BIT5,
	SPEED1 = BIT6,
	ADC_POWER = BIT7,
};

enum {
	T90  = SPEED1 | SPEED0,
	T180 = SPEED1,
	T360 = SPEED0,
	T540 = 0,
};

#define ADC_CONTR_VAL	(ADC_POWER | T90 | ADC_START | 1 )
static u16 adc_val = 0;
void adc_isr() interrupt 5 using 2 {
	ADC_CONTR &= (~ADC_FLAG);
	adc_val = ((ADC_RES & 0x3) << 8) | ADC_RESL;
}

void start_once_adc(void) {
	while (ADC_CONTR & ADC_START)
		return;
	printf("adc val = %u\n", adc_val);
	ADC_CONTR = ADC_CONTR_VAL;
	adc_val = 0;
}

void infrared_init(void) {

	//P1M1 |= BIT0;
	//P1M0 &= ~BIT0;
	P1M0 |= BIT1;
	P1M1 |= (BIT0 | BIT1);
	P1ASF = BIT1;			//P1.3设置为复用，作为模拟A/D输入口
	
	AUXR1 |= BIT2;			//ADC_RES[1:0], ADC_RESL[7:0] 作为10位值
	EADC = 1;				//启用ADC中断
	ADC_CONTR = ADC_CONTR_VAL;	//启用P1.0作为AD输入，90个时钟周期一次转换
}