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
	P1ASF = BIT1;			//P1.3����Ϊ���ã���Ϊģ��A/D�����
	
	AUXR1 |= BIT2;			//ADC_RES[1:0], ADC_RESL[7:0] ��Ϊ10λֵ
	EADC = 1;				//����ADC�ж�
	ADC_CONTR = ADC_CONTR_VAL;	//����P1.0��ΪAD���룬90��ʱ������һ��ת��
}