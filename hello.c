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

   EA = 1; //�������жϣ��粻ʹ���жϣ�����//���Σ�
   ES = 1; //����UART���ڵ��ж�

   //ò��һ���жϣ��ͻ����һ�������ж�,
   //����������ʼ�����ջ�����
   usart_recv(); 

   while (1) {  		
		if (USART_RCV_FLAG)
			 printf("usart recv:%s", usart_recv());
   }
}