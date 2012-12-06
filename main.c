#include <STC12C5A60S2.H>
#include "lib.h"
#include "i2c.h"
#include "usart.h"
#include "string.h"
#include "spi.h"
#include "led.h"
#include "nrf24l01.h"
#include "zx12864.h"
#include "infrared.h"
#include "DS18B20.h"
#include <stdio.h>
#include "lcd.h"

void main(void) {

#ifdef SENDER
   char msg[50];
   int i = 0;
#endif

   delay_us(480);

   usart_init();
   delay_init();
   delay_ms(450);
   printf("os start\n");

   //infrared_init();
   //spi_init();
   //init_ds18b20();
//#ifdef RECVER
   //zx12864_init();
//#endif
   //jlx_test();	//��Ļ����
#ifdef SENDER    
   led_init();
   i2c_init();
   led_print("OS Start!");
#endif

   //init_nrf24l01();
   //init_ds18b20();
   //EA = 1; //�������жϣ��粻ʹ���жϣ�����//���Σ�
   //ES = 1; //����UART���ڵ��ж�
   
   //ò��һ���жϣ��ͻ����һ�������ж�,
   //����������ʼ�����ջ�����
   //usart_recv(); 

#ifdef RECVER   
   recv_status();
#endif
   LCD_Lib_Initial();
   print_test();

   while (1) {
   		//printf("read tmp = %f.\n", read_tmp());  		
		//if (USART_RCV_FLAG)
		//	printf("usart recv:%s", usart_recv());
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
		//start_once_adc();
#ifdef SENDER	   
	   sprintf(msg, "send via wireless: %d", i);
	   printf("usart: %s\n", msg);
	   led_clr();
	   led_print(msg);
	   nrf24l01_send(msg, strlen(msg));
	   i++;
#endif 
   }
}