#include <STC12C5A60S2.H>
#include "lib.h"
#include "i2c.h"
#include "usart.h"
#include "string.h"
#include "spi.h"
#include "led.h"
#include "nrf24l01.h"
#include <stdio.h>

void main(void) {
  
   usart_init();
   delay_ms(150);
   printf("os start\n");
   
   spi_init();   
   led_init();
   i2c_init();
   led_print("OS Start!");
   init_nrf24l01();

   EA = 1; //允许总中断（如不使用中断，可用//屏蔽）
   ES = 1; //允许UART串口的中断

   //貌似一打开中断，就会产生一个串口中断,
   //这个函数会初始化接收缓冲区
   usart_recv(); 

#ifdef RECVER   
   recv_status();
#endif

   while (1) {  		
		if (USART_RCV_FLAG)
			printf("usart recv:%s", usart_recv());
		if (send_flag) {
			printf("send_flag\n");
			send_flag = FALSE;	
		}
		if (recv_flag) {
			printf("recv_flag: %s\n", nrf24l01_recv());
			recv_flag = FALSE;	
		}
		if (rety_flag) {
			printf("rety_flag\n");
			rety_flag = FALSE;	
		}
#ifdef SENDER
	   delay_ms(40);
	   led_clr();
	   delay_ms(40);
	   led_print("send_msg");
	   nrf24l01_send("send from wireless\n", strlen("send from wireless\n"));
#endif 
   }
}