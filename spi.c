#include <STC12C5A60S2.H>
#include "lib.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

sbit MISO 	= P1 ^ 6;
sbit MOSI 	= P1 ^ 5;
sbit SPICLK = P1 ^ 7;
sbit SS		= P1 ^ 4;

//SPDAT	: ���ݼĴ���
//SPCTL ��SPI ���ƼĴ���
//SPSTAT��SPI ״̬�Ĵ���

enum {
	SPR0 = BIT0,
	SPR1 = BIT1,
	CPHA = BIT2,
	CPOL = BIT3,
	MSTR = BIT4,
	DORD = BIT5,
	SPEN = BIT6,
	SSIG = BIT7,
	SPIF = BIT7,	//SPI ������� ��־
	ECOL = BIT6,	//SPIд��ͻ	
};

#define DISABLE_SPI 	(SPCTL &= (~SPEN))
#define ENABLE_SPI		(SPCTL |= SPEN)
#define CLR_BIT_SPI(x) 	(SPCTL &= (~(x)))
#define SET_BIT_SPI(x)	(SPCTL |= (x))

//Ĭ�����ã�����SPI��Ϊ���豸�� SS���Ų���, P1.5,P1.7 Ϊ�����P1.6Ϊ����
//1/4��Ƶ���ȷ���λ������ʱSPICLKΪ�͵�ƽ
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


static char  xdata page_buf[PAGE_SIZE];	//ҳ�����ַ
//���������ַ��������

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

//����Ҫ�ӵ�ַ123������256����513...���ֽڣ�
int spi_read(u32 addr, char *dst, u16 buf_size) {
	u16 start = addr, end = addr + buf_size;
	u32 i;

	//if ((start & 0xffff00) == (end & 0xffff00))	//˵����ͬһҳ��
	{
		spi_read_page(start & 0xffff00);
		memcpy(dst, &page_buf[start & 0x0000ff], buf_size);
		return;	
	}
	
//	//�ȿ�ͷ
//	spi_read_page(start & 0xffff00);
//	memcpy(dst, &page_buf[start & 0x0000ff], PAGE_SIZE - (start & 0x0000ff));
//	//�ٿ�β
//	spi_read_page(end & 0xffff00);
//	memcpy((char *)((end >> 8) * PAGE_SIZE) , &page_buf[end & 0x0000ff], end & 0x0000ff);
//	//�м�
//	start = ((start + 0xff) / PAGE_SIZE) * PAGE_SIZE ;	//���϶���
//	dst	  = ((d + 0xff) / PAGE_SIZE) * PAGE_SIZE ;	//���϶���
//	end	  = end & 0xffff00;								//���¶���
//	for (i = start; i < end; i += PAGE_SIZE) {
//		 spi_read_page(i & 0xffff00);
//		 memcpy((char *)i, page_buf, PAGE_SIZE);
//	}	
}

//һ�����д256���ֽ�
//һ�����ٲ�д4K
//д����Ϊ���ʱBUSY��1,��ʱӲ���Զ���0


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

	if ((start & 0xffff00) == (end & 0xffff00))	//˵����ͬһҳ��
	{
		spi_read_page(start);
		spi_erase_page(addr);
		memcpy(&page_buf[start & 0x0000ff], src, len);
		spi_write_page(addr, page_buf);
		return;	
	}

//	//��дͷ
//	spi_read_page(start);
//	spi_erase_page(start);
//	memcpy(&page_buf[start & 0x0000ff], start, PAGE_SIZE - (start & 0x0000ff));
//	spi_write_page(start, page_buf);
//	//��дβ
//	spi_read_page(end);
//	spi_erase_page(end);
//	memcpy(&page_buf[end & 0x0000ff], (char *)((end >> 8) * PAGE_SIZE) ,  end & 0x0000ff);
//	spi_write_page(end, page_buf);
//	//Ȼ�����м�
//	start = ((start + 0xff) / PAGE_SIZE) * PAGE_SIZE ;	//���϶���
//	end	  = end & 0xffff00;								//���¶���
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