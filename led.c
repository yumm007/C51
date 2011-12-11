#include <STC12C5A60S2.H>
#include <string.h>
#include <intrins.h>
#include <stdio.h>
#include "lib.h"
#include "led.h"
#include "usart.h"

sbit RS = P0 ^ 7;	 //����Ϊ1������Ϊ0
sbit RW = P0 ^ 6;	 //��Ϊ1��дΪ0
sbit CS	= P0 ^ 5;	 //ƬѡΪ1

#define  LEDDATA P2

static void led_ready(void) {
	u8 val;
	
	do {
		RS = 0;
		RW = 1;
		LEDDATA = 0xff;
		_Nop();
		CS = 1;
		_Nop();
		val = LEDDATA;
		CS = 0;
	} while (val & 0x80);
}

static void led_send_cmd(u8 cmd) {
	led_ready();
	RS = 0;
	RW = 0;
	LEDDATA = cmd;
	CS = 1;
	_Nop();
	CS = 0;	
}

static void led_send_data(u8 val) {
	led_ready();	
	RS = 1;
	RW = 0;
	LEDDATA = val;
	CS = 1;
	_Nop();
	CS = 0;	
}

void led_print(char *str) {
	led_send_cmd(0x06); //�Զ���λ
	while (*str) {
		led_send_data(*str++);
	}
}

void led_init(void) {

	P2M0 = 0x00;
	P2M1 = 0x00;
	delay_ms(10);

	led_send_cmd(0x38);	//8λ��ʽ��2�У�5*7
	led_send_cmd(0x08);	//�ر���ʾ
	led_send_cmd(0x01);	//�����ʾ
	
	led_send_cmd(0x06);	//���������ʽ����������λ
	led_send_cmd(0x0c);	//������ʾ���ع�꣬����˸
	
	led_send_cmd(0x80); //��ʾλ��

	return;
}