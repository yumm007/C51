#include <STC12C5A60S2.H>
#include <string.h>
#include <intrins.h>
#include <stdio.h>
#include "lib.h"
#include "led.h"
#include "usart.h"
#include "spi.h"
#include "nrf24l01.h"

sbit NRF_CS = P3 ^ 4;		//SPI片选
sbit NRF_CE	= P3 ^ 5;		//发送还是接受模式

#define STATUS		0x00	//状态寄存器
#define RX_ADDR_P0	0x0A 	//通道0的地址

#define R_REGISTER	0x00
#define W_REGISTER	0x10

enum {

	PRIM_RX		= BIT0,
	PWR_UP 		= BIT1,
};

static void power_down(void) {
	u8 val;
	NRF_CS = 0;
//	spi_send_byte(R_REGISTER(STATUS));
//	val = spi_recv_byte();
//	val &= ~PWR_UP;
//	spi_send_byte(R_REGISTER(STATUS));
//	spi_send_byte(val);
	NRF_CS = 1;	
}

static void sleep_mode(void) {
	NRF_CE = 0;
	delay_ms(1);
}

static void print_pipi0_addr(void) {
	u8 val = 0;
	NRF_CS = 0;
	//spi_send_byte(0xA5);
	spi_send_byte(0xa);
	//delay_ms(1);
	//spi_send_byte(5);
	//spi_send_byte(0x01);
	//add[4] = spi_recv_byte();
	//add[3] = spi_recv_byte();
	//add[2] = spi_recv_byte();
	//add[1] = spi_recv_byte();
	val = spi_recv_byte();
	NRF_CS = 1;
	printf("nRF24L01 reg val: 0x%x\n",(u32)val);
}

void init_nrf24l01(void) {
	
	//NRF_CS = 0;

	//while(1);

	//sleep_mode();
	print_pipi0_addr();
}
