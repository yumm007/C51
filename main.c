#include <STC12C5A60S2.H>
#include "lib.h"
#include "i2c.h"
#include "usart.h"
#include "string.h"
#include "spi.h"
#include "led.h"
#include "nrf24l01.h"
#include <stdio.h>

sbit INT0 = P3 ^ 2;

void main(void) {
  
   usart_init();
   delay_ms(150);
   printf("os start\n");
   
   spi_init();   
   led_init();
   i2c_init();
   led_print("OS Start!");
   init_nrf24l01();

   EA = 1; //�������жϣ��粻ʹ���жϣ�����//���Σ�
   ES = 1; //����UART���ڵ��ж�

   //ò��һ���жϣ��ͻ����һ�������ж�,
   //����������ʼ�����ջ�����
   usart_recv(); 

   nrf24l01_send("send from wireless\n", strlen("send from wireless\n"));

   while (1) {  		
		if (USART_RCV_FLAG)
			printf("usart recv:%s", usart_recv());
		if (send_flag) {
			printf("send_flag\n");
			send_flag = FALSE;	
		}
		if (recv_flag) {
			printf("recv_flag\n");
			recv_flag = FALSE;	
		}
		if (rety_flag) {
			printf("rety_flag\n");
			rety_flag = FALSE;	
		}

   }
}