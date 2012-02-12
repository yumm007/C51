#include "lib.h"
#include <STC12C5A60S2.H>

void delay_init(void) {

	TMOD &= 0xf0;
	TMOD |= BIT0; //��ʱ��0Ϊ16λ��ʱ����

	AUXR |= BIT7;	  //T0x12 = 1 1Tģʽ����ÿ��ʱ�����ڶ�Ӧһ���������ڣ�ÿ���������ڼ�ʱ��1.
	
	ET0 = 1;
	EA  = 1;
}


#if 1

#define T01MS	(0xffff - FOSC / 1000)
#define T01US	(0xffff - FOSC / 1000 /1000)

static u8 tm0_running = 0;
void tm0_isr(void) interrupt 1 {
	tm0_running = 0;	
} 

void delay_ms(unsigned int n) {

	while (n--) {
		//1�����ʱ��
		TL0 = T01MS;
		TH0 = (T01MS >> 8);

		tm0_running = 1;
		
		TR0 = 1;	//������ʱ��

		while (tm0_running)
			;
	}
	
	TR0 = 0;	
}

//void delay_us(unsigned int n) {
//	u16 t;
//
//	t =  T01US - 4 - 1 - 2 * n - 4;
//
//	while (n--) {
//		//1΢���ʱ��
//		TL0 = t;
//		TH0 = (t >> 8);
//
//		tm0_running = 1;
//		
//		TR0 = 1;	//������ʱ��
//
//		while (tm0_running)
//			;
//	}
//	
//	TR0 = 0;
//
//}

#else

void delay_ms(unsigned int n) {
	unsigned int i;

	while (n--)
		for (i = 0; i < 600; i++)
			;
}

#endif

void delay_us(unsigned int n) {	
	//һ��whileѭ��6��ָ����ÿ��ָ����Ҫ2.5��ʱ������, ��ÿѭ��һ��1us
	while (n--) {
		;
	}
}

