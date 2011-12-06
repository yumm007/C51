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
#define PAGE_SIZE	256

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


static char  xdata page_buf[PAGE_SIZE];	//页缓存地址
//考虑随机地址读得问题

static spi_read_page(u32 addr) {
	u16 i = 0;
	SS = 0;
	spi_send_byte(FAST_READ);
	spi_send_byte((addr & 0xff0000) >> 16);
	spi_send_byte((addr & 0x00ff00) >> 8);
	spi_send_byte(0);
	spi_send_byte(SPI_DUMMY);
	for (i = 0; i < PAGE_SIZE; i++)
		page_buf[i] =  spi_recv_byte();
	SS = 1;	
}

//比如要从地址123连续读256或者513...个字节，
int spi_read(u32 addr, char *dst, u16 buf_size) {
	u16 start = addr, end = addr + buf_size;
	u32 i;

	//if ((start & 0xffff00) == (end & 0xffff00))	//说明在同一页中
	{
		spi_read_page(start & 0xffff00);
		memcpy(dst, &page_buf[start & 0x0000ff], buf_size);
		return;	
	}
	
//	//先拷头
//	spi_read_page(start & 0xffff00);
//	memcpy(dst, &page_buf[start & 0x0000ff], PAGE_SIZE - (start & 0x0000ff));
//	//再拷尾
//	spi_read_page(end & 0xffff00);
//	memcpy((char *)((end >> 8) * PAGE_SIZE) , &page_buf[end & 0x0000ff], end & 0x0000ff);
//	//中间
//	start = ((start + 0xff) / PAGE_SIZE) * PAGE_SIZE ;	//向上对齐
//	dst	  = ((d + 0xff) / PAGE_SIZE) * PAGE_SIZE ;	//向上对齐
//	end	  = end & 0xffff00;								//向下对齐
//	for (i = start; i < end; i += PAGE_SIZE) {
//		 spi_read_page(i & 0xffff00);
//		 memcpy((char *)i, page_buf, PAGE_SIZE);
//	}	
}

//一次最多写256个字节
//一次最少擦写4K
//写操作为完成时BUSY置1,闲时硬件自动清0


#define WEN 	spi_write_opt(TRUE)
#define WDIS	spi_write_opt(FALSE)
static void spi_write_opt(bool val) {
	SS = 0;
	spi_send_byte(val ? WRITE_ENABLE : WRITE_DISABLE);
	SS = 1;	
}

static void spi_erase_page(u32 addr) {
	u8 ret;
	WEN;
	SS = 0;
	spi_send_byte(SECTOR_ERASE);
	spi_send_byte((addr & 0xff0000) >> 16);
	spi_send_byte((addr & 0x00ff00) >> 8);
	spi_send_byte(0);
	SS = 1;

	do {
	   ret = spi_sta();
	} while (ret & SPI_BUSY);	
}

static void spi_write_page(u32 addr, const char *src) {
	u8 ret;
	u16 i;
	WEN;

	SS = 0;
	spi_send_byte(PAGE_PROGRAM);
	spi_send_byte((addr & 0xff0000) >> 16);
	spi_send_byte((addr & 0x00ff00) >> 8);
	spi_send_byte((addr & 0x0000ff) >> 0);	
	for (i = 0; i < PAGE_SIZE; i++)
		spi_send_byte(src[i]);
	SS = 1;

	do {
		ret = spi_sta();
	} while (ret & SPI_BUSY);	
}

int spi_write(u32 addr, const char *src, u16 len) {
	u16 start = addr, end = addr + len;
	u32 i;

	if ((start & 0xffff00) == (end & 0xffff00))	//说明在同一页中
	{
		spi_read_page(start);
		spi_erase_page(addr);
		memcpy(&page_buf[start & 0x0000ff], src, len);
		spi_write_page(addr, page_buf);
		return;	
	}

//	//先写头
//	spi_read_page(start);
//	spi_erase_page(start);
//	memcpy(&page_buf[start & 0x0000ff], start, PAGE_SIZE - (start & 0x0000ff));
//	spi_write_page(start, page_buf);
//	//再写尾
//	spi_read_page(end);
//	spi_erase_page(end);
//	memcpy(&page_buf[end & 0x0000ff], (char *)((end >> 8) * PAGE_SIZE) ,  end & 0x0000ff);
//	spi_write_page(end, page_buf);
//	//然后是中间
//	start = ((start + 0xff) / PAGE_SIZE) * PAGE_SIZE ;	//向上对齐
//	end	  = end & 0xffff00;								//向下对齐
//	for (i = start; i < end; i += PAGE_SIZE) {
//		 spi_read_page(i);
//		 spi_erase_page(i);
//		 memcpy(page_buf, (char *)i, PAGE_SIZE);
//		 spi_write_page(i, page_buf);
//	}
}

void spi_deinit(void) {
	DISABLE_SPI;
}

void spi_init(void) {
	char xdata tmp[50] = "aaaa aaaa aaaa\n";

	SPCTL 	= 	SPI_DEF_VAL;
	SPSTAT 	=  	SPIF | ECOL;

	printf("\"ef16\"\t.spi device id:0x%x, status: %d\n", spi_device_id(), (u32)spi_sta());

	spi_write(0, "this line will be write to spi flash.\n", strlen("this line will be write to spi flash.\n"));
	//delay_ms(1000);
	spi_read(0, tmp , strlen("this line will be write to spi flash.\n"));
	printf("%s", tmp);
}