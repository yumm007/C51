#include "lib.h"
#include "usart.h"
#include "i2c.h"
#include <STC12C5A60S2.H>

static void usart_rcv_(void ) interrupt 4 using 1 {
//	unsigned char UART_data; //���崮�ڽ������ݱ���
//	RI = 0;			//������жϱ�־λΪ0��������㣩
//	UART_data = SBUF;	//�����յ�������������� UART_data
//		
//	SBUF = UART_data;	//�����յ����ݷ��ͻ�ȥ��ɾ��//����Ч��
//	while(TI == 0);	//��鷢���жϱ�־λ
//	TI = 0;		//����жϱ�־λΪ0��������㣩
}

void usart_init(void) {
	//EA = 1; //�������жϣ��粻ʹ���жϣ�����//���Σ�
	//ES = 1; //����UART���ڵ��ж�

	//AUXR = 0x40;	//1Tģʽ
	TMOD = 0x20;	//��ʱ��1 ������ʽ2
	SCON = 0x50;	//���ڹ�����ʽ1�������ڽ��գ�SCON = 0x40 ʱ��ֹ���ڽ��գ�
	TH1 = 0xf3;	//��ʱ����ֵ��8λ����
	TL1 = 0xf3;	//��ʱ����ֵ��8λ����
	PCON = 0x80;	//�����ʱ�Ƶ�����α��䲨����Ϊ2400��
	TI	= 1;
	TR1 = 1;	//��ʱ������

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

void usart_recv(char *buf, int n) {
	;
}
