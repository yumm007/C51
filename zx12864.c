#include "zx12864.h"
#include <STC12C5A60S2.H>
#include <string.h>
#include <intrins.h>
#include <stdio.h>
#include "lib.h"
#include "usart.h"

sbit ZRS = P2 ^ 0;	//�����������
sbit ZRW = P2 ^ 1;	//������д
sbit ZCS = P2 ^ 2;	//Ƭѡ
//sbit ZRST= P2 ^ 3;	//��λ

#define LEDDATA P0

enum {CMD 		= 0, DATA};
enum {WRITE		= 0, READ};
enum {DISABLE 	= 0, ENABLE};


static void led_ready() {
	u8 val;
	LEDDATA = 0xff;
	do {
		ZCS = 0;
		ZRS = CMD;
		ZRW = READ;
		
		_Nop();
		ZCS = 1;
		_Nop();		
		ZCS = 0; 
		val = LEDDATA;

	} while (val & 0x80);
}
static void led_set(u8 val) {
	led_ready();
    ZCS = DISABLE;
	ZRS = CMD;
	ZRW = WRITE;
	_Nop();
	LEDDATA = val;
	ZCS = ENABLE;
	_Nop();
	ZCS = DISABLE;	
	_Nop();
}

static void led_write(u8 val) {
	led_ready();
    ZCS = DISABLE;
	ZRS = DATA;
	ZRW = WRITE;
	LEDDATA = val;
	_Nop();
	ZCS = ENABLE;
	_Nop();
	ZCS = DISABLE;	
	_Nop();
}

static u8 led_read(void) {
	led_ready();
	ZCS = DISABLE;
	ZRS = DATA;
	ZRW = READ;
	ZCS = ENABLE;
	_Nop();
	ZCS = DISABLE;
	return LEDDATA;
}

void zled_print(char *s) {
	while (*s)
		led_write(*s++);
}

void zx12864_init(void) {

	P0M0 = 0x00;
	P0M1 = 0x00;

	led_set(0x30);	//����
	led_set(0x0c);
	led_set(0x01);
	zled_print("ϵͳ������");

	return;
}
