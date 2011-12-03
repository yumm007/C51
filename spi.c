#include <STC12C5A60S2.H>
#include "lib.h"
#include "usart.h"
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

void spi_deinit(void) {
	DISABLE_SPI;
}

void spi_init(void) {
	SS = 1;
	SPCTL 	= 	SPI_DEF_VAL;
	SPSTAT 	=  	SPIF | ECOL;
	printf("\"ef16\"\t.spi device id:0x%x\n", spi_device_id());
}