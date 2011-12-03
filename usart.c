#include "lib.h"
#include "usart.h"
#include "i2c.h"
#include <STC12C5A60S2.H>

static void usart_rcv_(void ) interrupt 4 using 1 {
//	unsigned char UART_data; //定义串口接收数据变量
//	RI = 0;			//令接收中断标志位为0（软件清零）
//	UART_data = SBUF;	//将接收到的数据送入变量 UART_data
//		
//	SBUF = UART_data;	//将接收的数据发送回去（删除//即生效）
//	while(TI == 0);	//检查发送中断标志位
//	TI = 0;		//令发送中断标志位为0（软件清零）
}

void usart_init(void) {
	//EA = 1; //允许总中断（如不使用中断，可用//屏蔽）
	//ES = 1; //允许UART串口的中断

	//AUXR = 0x40;	//1T模式
	TMOD = 0x20;	//定时器1 工作方式2
	SCON = 0x50;	//串口工作方式1，允许串口接收（SCON = 0x40 时禁止串口接收）
	TH1 = 0xf3;	//定时器初值高8位设置
	TL1 = 0xf3;	//定时器初值低8位设置
	PCON = 0x80;	//波特率倍频（屏蔽本句波特率为2400）
	TI	= 1;
	TR1 = 1;	//定时器启动

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

void usart_recv(char *buf, int n) {
	;
}
