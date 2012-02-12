#ifndef __LIB_H_
#define __LIB_H_

#include <intrins.h>

typedef enum {FALSE = 0, TRUE = !FALSE} bool;
typedef unsigned char uchar;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#ifndef NULL
#define NULL	0
#endif

#define FOSC	12000000L

enum {
	BIT0 = 1<<0,
	BIT1 = 1<<1,
	BIT2 = 1<<2,
	BIT3 = 1<<3,
	BIT4 = 1<<4,
	BIT5 = 1<<5,
	BIT6 = 1<<6,
	BIT7 = 1<<7,
};

#define _Nop() _nop_() /*定义空指令*/

void delay_init(void);
void delay_ms(unsigned int n);
void delay_us(unsigned int n);

//for循环体累加4条，比较3条，跳转1条，初始化4条；

#define delay_us_1(n) {\
	unsigned int x = n;	\
	while (x--)	\
		;	\
	}
	
#endif