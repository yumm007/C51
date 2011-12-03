#ifndef __LIB_H_
#define __LIB_H_

typedef enum {TRUE = 1, FALSE = 0} bool;
typedef unsigned char uchar;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define NULL	0

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

void delay_ms(unsigned int n);

#endif