#include <STC12C5A60S2.H>
#include "lib.h"
#include "i2c.h"
#include "usart.h"
#include "string.h"
#include "spi.h"
#include <stdio.h>

void main(void) {
  
   usart_init();
   delay_ms(150);
   printf("os start\n");

   spi_init();
   i2c_init();

   EA = 1; //允许总中断（如不使用中断，可用//屏蔽）
   ES = 1; //允许UART串口的中断

   //貌似一打开中断，就会产生一个串口中断,
   //这个函数会初始化接收缓冲区
   usart_recv(); 

   while (1) {  		
		if (USART_RCV_FLAG)
			 printf("usart recv:%s", usart_recv());
   }
}