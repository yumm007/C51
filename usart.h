#ifndef __LIB_USART_H__
#define __LIB_USART_H__

#define FOSC 12000000L
#define BAUD (256-FOSC/32/115200)

void usart_init(void);
void usart_send(const char *buf, int n);
void usart_recv(char *buf, int n);

#endif