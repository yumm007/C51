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

#define R_REGISTER(x)	(0x00 + (x))
#define W_REGISTER(x)	(0x20 + (x))
#define R_RX_PAYLOAD	0x61
#define W_RX_PAYLOAD	0xA0
#define FLUSH_TX        0xE1

static xdata char nrfbuf[32];
static xdata char tar_addr[5] = TAR_ADDR; 

enum {

	PRIM_RX		= BIT0,
	PWR_UP 		= BIT1,
};

static void print_pipi0_addr(void) {
	u8 val = 0;
	NRF_CS = 0;
	spi_send_byte(R_REGISTER(0x6));
	val = spi_recv_byte();
	NRF_CS = 1;
	printf("nRF24L01 reg val: 0x%x\n",(u32)val);
}


static void write_reg(u8 cmd, u8 val) {
	NRF_CS = 0;
	spi_send_byte(cmd);
	spi_send_byte(val);
	NRF_CS = 1;
}

static u8 read_reg(u8 cmd) {
	u8 ret = 0;
	NRF_CS = 0;
	spi_send_byte(cmd);
	ret = spi_recv_byte();
	NRF_CS = 1;
	return ret;
}

static void write_data(u8 cmd, char *src, u8 len) {
	int i;
	NRF_CS = 0;
	spi_send_byte(cmd);
	for (i = 0; i < len; i++)
		spi_send_byte(src[i]);
	NRF_CS = 1;
}

static void recv_status(void) {
	NRF_CE = 0;
	write_data(W_REGISTER(0x0A), tar_addr, 5);	//数据通道0的接收地址
	write_reg(W_REGISTER(0x02), 0x01);	 		//通道0设为接收模式	
	write_reg(W_REGISTER(0x01), 0x01);	 		//允许通道0自动应答

	write_reg(W_REGISTER(0x00), 0x0f);
	NRF_CE = 1;
}

static void send_status(void) {

	NRF_CE = 0;

	write_data(W_RX_PAYLOAD, nrfbuf, 32);		//写入需要发送的数据
	write_data(W_REGISTER(0x10), tar_addr, 5); 	//写入需要发送的地址
	write_data(W_REGISTER(0x0A), tar_addr, 5);	//数据通道0的接收地址，和上面的一样

	write_reg(W_REGISTER(0x02), 0x01);	 		//通道0设为接收模式	
	write_reg(W_REGISTER(0x01), 0x01);	 		//允许通道0自动应答
		
	write_reg(W_REGISTER(0x00), 0x0e);	 		//改成发送模式
	
	NRF_CE = 1;							 		//启动发射
}

bool send_flag = FALSE, recv_flag = FALSE, rety_flag = FALSE;

static void nrf24l01_rcv_(void ) interrupt 0 using 0 {
	u8 val = read_reg(0x07);
	
	if (val & (1<<5))	    //发送完成中断
		send_flag = TRUE;	
	if (val & (1<<6))		//接收到数据中断
		recv_flag = TRUE;
	if (val & (1<<4)) {
		rety_flag = TRUE;
		write_reg(FLUSH_TX, 0);
	}
	
	write_reg(W_REGISTER(0x07), 0xff);	
}

void nrf24l01_send(const char *src, u8 len) {
	memset(nrfbuf, 0, 32);
	memcpy(nrfbuf, src, len < 32 ? len : 32);
	send_status();
}

void init_nrf24l01(void) {

	write_reg(W_REGISTER(0x04), 0x1a); 			//自动重发
	write_reg(W_REGISTER(0x05), 40); 			//射频通道40
	write_reg(W_REGISTER(0x06), 7); 			//1Mbps，0dbm
	write_reg(W_REGISTER(0x11), 32); 			//32字节有效数据
	write_reg(W_REGISTER(0x12), 32); 			//32字节有效数据	
	
	print_pipi0_addr();
	EX0 = 1;									//打开外部中断0
}
