#include <STC12C5A60S2.H>
#include "lib.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

sbit MISO 	= P1 ^ 6;
sbit MOSI 	= P1 ^ 5;
sbit SPICLK = P1 ^ 7;
sbit SS		= P1 ^ 4;

//SPDAT	: 数据寄存器
//SPCTL ：SPI 控制寄存器
//SPSTAT：SPI 状态寄存器

enum {
	SPR0 = BIT0,
	SPR1 = BIT1,
	CPHA = BIT2,
	CPOL = BIT3,
	MSTR = BIT4,
	DORD = BIT5,
	SPEN = BIT6,
	SSIG = BIT7,
	SPIF = BIT7,	//SPI 传输完成 标志
	ECOL = BIT6,	//SPI写冲突	
};

#define DISABLE_SPI 	(SPCTL &= (~SPEN))
#define ENABLE_SPI		(SPCTL |= SPEN)
#define CLR_BIT_SPI(x) 	(SPCTL &= (~(x)))
#define SET_BIT_SPI(x)	(SPCTL |= (x))

//默认配置：启用SPI，为主设备， SS引脚不用, P1.5,P1.7 为输出，P1.6为输入
//1/4分频，先发高位，空闲时SPICLK为低电平
#define SPI_DEF_VAL (0x00 | SSIG | MSTR | SPEN | CPOL | CPHA)
#define SPI_DUMMY	0x5a

static u8 spi_send_byte(u8 val) {

	SPSTAT = SPIF | ECOL;
	SPDAT = val;

	while  (!(SPSTAT & 0x80)) {
	}

	return SPDAT;
}

static u8 spi_recv_byte(void) {
	return spi_send_byte(SPI_DUMMY);
}

static u16 spi_device_id(void) {
	u16 ret = 0;

	SS = 0;
		
	spi_send_byte(0x90);
		
	spi_send_byte(0x00);
	spi_send_byte(0x00);
	spi_send_byte(0x00);

	ret = spi_recv_byte() << 8;
	ret |= spi_recv_byte() << 0;
	SS = 1;

	return ret;
}



#define WRITE_ENABLE 	0x06
#define WRITE_DISABLE	0x04
#define READ_STA1		0x05
#define READ_STA2		0x35
#define WRITE_STA		0x01
#define SECTOR_ERASE	0x20
#define	FAST_READ		0x0B
#define	PAGE_PROGRAM	0x02

#define SPI_BUSY		BIT0

static u8 spi_sta(void) {
	u8 ret = 0;
	SS = 0;
	spi_send_byte(READ_STA1);
	ret = spi_recv_byte();
	SS = 1;
	return ret;
}

int spi_read(u16 addr, char *dst, u16 buf_size) {
	SS = 0;
	spi_send_byte(0x03);
	spi_send_byte(addr & 0xff0000);
	spi_send_byte(addr & 0x00ff00);
	spi_send_byte(addr & 0x0000ff);
	spi_send_byte(SPI_DUMMY);
	
	while (buf_size--)
		*dst++ = spi_recv_byte();

	SS = 1;	
}

//一次最多写256个字节
//一次最少擦写4K
//写操作为完成时BUSY置1,闲时硬件自动清0
//static char xdata page_buf[256];

#define WEN 	spi_write_opt(TRUE)
#define WDIS	spi_write_opt(FALSE)
static void spi_write_opt(bool val) {
	SS = 0;
	spi_send_byte(val ? WRITE_ENABLE : WRITE_DISABLE);
	SS = 1;	
}

int spi_write(u16 addr, const char *src, u16 len) {
	u16 i = 0;
	u8 ret = 1;
	//memset(page_buf, 0, sizeof(page_buf));
	WEN;

	SS = 0;
	spi_send_byte(SECTOR_ERASE);
	spi_send_byte(addr & 0xff0000);
	spi_send_byte(addr & 0x00ff00);
	spi_send_byte(addr & 0x0000ff);

	do {
	   ret = spi_sta();
	} while (ret & SPI_BUSY);	

	SS = 0;
	spi_send_byte(PAGE_PROGRAM);
	spi_send_byte(addr & 0xff0000);
	spi_send_byte(addr & 0x00ff00);
	spi_send_byte(addr & 0x0000ff);	

	for (i = 0; i < 256; i++)
		spi_send_byte(i < len ? src[i] : 0);
	
	//delay_ms(100);

	do {
	   ret = spi_sta();
	} while (ret & SPI_BUSY);

	SS = 1;
	WDIS;
}

void spi_deinit(void) {
	DISABLE_SPI;
}

void spi_init(void) {
	char xdata tmp[50] = "aaaa aaaa aaaa\n";

	SPCTL 	= 	SPI_DEF_VAL;
	SPSTAT 	=  	SPIF | ECOL;

	printf("\"ef16\"\t.spi device id:0x%x\n", spi_device_id());

	//spi_write(0, "this line will be write to spi flash.\n", strlen("this line will be write to spi flash.\n"));
	//delay_ms(1000);
	spi_read(0, tmp , strlen("this line will be write to spi flash.\n"));
	printf("%s", tmp);
}