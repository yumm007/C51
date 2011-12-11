#include <STC12C5A60S2.H>
#include <string.h>
#include <intrins.h>
#include <stdio.h>
#include "lib.h"
#include "led.h"
#include "usart.h"

sbit RS = P0 ^ 7;	 //数据为1，命令为0
sbit RW = P0 ^ 6;	 //读为1，写为0
sbit CS	= P0 ^ 5;	 //片选为1

#define  LEDDATA P2

static u8 led_offset = 0;

static u8 led_ready(void) {
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
	while (*str) {
		led_send_data(*str++);
		led_offset++;
		if (led_offset > 0x0f && !(led_offset & 0x40))
			set_xy(1,0);
		else if (led_offset > 0x4f)
			set_xy(0,0);
	}
}

void set_xy(u8 x, u8 y) {
	u8 add = (x == 0 ? 0 : 0x40);
	add |= (y < 16 ? y : 16);

	led_offset = add;
	led_send_cmd(0x80 | add);

}

void led_clr(void) {
	led_send_cmd(0x01);
	led_offset = 0;
}

void led_init(void) {

	P2M0 = 0x00;
	P2M1 = 0x00;
	delay_ms(10);

	led_send_cmd(0x38);	//8位格式，2行，5*7
	led_send_cmd(0x08);	//关闭显示
	
	led_send_cmd(0x06);	//设置输入格式，增量不移位
	led_send_cmd(0x0c);	//整体显示，关光标，不闪烁
	
	led_clr();			//清除显示

	return;
}