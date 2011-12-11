#ifndef __LIB_USART_H__
#define __LIB_USART_H__

#define USART_BUF_SIZE	64

void usart_init(void);
void usart_send(const char *buf, int n);
char * usart_recv(void);
extern bool USART_RCV_FLAG;

#endif