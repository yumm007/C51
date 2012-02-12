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
	
	RI = 0;			//令接收中断标志位为0（软件清零）
	
	usart_buf[buf_p++] = rcv = SBUF;	//将接收到的数据送入变量 

	if (rcv == '\r')
		flag = TRUE;
	else if ((rcv == '\n' && flag == TRUE) || buf_p == USART_BUF_SIZE) {
		USART_RCV_FLAG = TRUE;
		flag	= FALSE;
	} else
		flag	= FALSE;	
}

void usart_init(void) {

	TMOD |= 0x20;	//定时器1 工作方式2
	SCON |= 0x50;	//串口工作方式1，允许串口接收（SCON = 0x40 时禁止串口接收）
	TH1 = 0xf3;	//定时器初值高8位设置
	TL1 = 0xf3;	//定时器初值低8位设置
	PCON |= 0x80;	//波特率倍频（屏蔽本句波特率为2400）
	TI	= 1;
	TR1 = 1;	//定时器启动

	buf_p = 0;
	USART_RCV_FLAG = FALSE;
}

void usart_send(const char *buf, int n) {
	while (n--) {		
		while (!TI)
			;
		TI = 0;
		switch (*buf) {
			case '\r' :			  //如果出现\r, 则直接输出\r和它之后的一个字符，可能是\n之类的
				SBUF = *(buf++);
				while (!TI)
					;
				TI = 0;
				SBUF = *(buf++);
				break;
			case '\n':			 //如果\n前头没有\r，则补录一个\r
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
