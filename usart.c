#include "lib.h"
#include "usart.h"
#include "i2c.h"
#include <STC12C5A60S2.H>

static u8  xdata usart_buf[USART_BUF_SIZE + 1] = {0};
static u8 buf_p = 0;

bool USART_RCV_FLAG = FALSE;
static bool flag = FALSE;

static void usart_rcv_(void ) interrupt 4 using 1 {
	u8 rcv;
	
	if (!RI) {
		return;
	}
	
	RI = 0;			//������жϱ�־λΪ0��������㣩
	
	usart_buf[buf_p++] = rcv = SBUF;	//�����յ�������������� 

	if (rcv == '\r')
		flag = TRUE;
	else if ((rcv == '\n' && flag == TRUE) || buf_p == USART_BUF_SIZE) {
		USART_RCV_FLAG = TRUE;
		flag	= FALSE;
	} else
		flag	= FALSE;	
}

void usart_init(void) {

	TMOD |= 0x20;	//��ʱ��1 ������ʽ2
	SCON |= 0x50;	//���ڹ�����ʽ1�������ڽ��գ�SCON = 0x40 ʱ��ֹ���ڽ��գ�
	TH1 = 0xf3;	//��ʱ����ֵ��8λ����
	TL1 = 0xf3;	//��ʱ����ֵ��8λ����
	PCON |= 0x80;	//�����ʱ�Ƶ�����α��䲨����Ϊ2400��
	TI	= 1;
	TR1 = 1;	//��ʱ������

	buf_p = 0;
	USART_RCV_FLAG = FALSE;
}

void usart_send(const char *buf, int n) {
	while (n--) {		
		while (!TI)
			;
		TI = 0;
		switch (*buf) {
			case '\r' :			  //�������\r, ��ֱ�����\r����֮���һ���ַ���������\n֮���
				SBUF = *(buf++);
				while (!TI)
					;
				TI = 0;
				SBUF = *(buf++);
				break;
			case '\n':			 //���\nǰͷû��\r����¼һ��\r
				SBUF = '\r';
				while (!TI)
					;
				TI = 0;
				SBUF = *(buf++);
				break;
			default:
				SBUF = *(buf++);
				break;		
		}
	}
}

char * usart_recv(void) {

	usart_buf[buf_p] = '\0';
	buf_p = 0;
	USART_RCV_FLAG = FALSE;

	return usart_buf;
}
