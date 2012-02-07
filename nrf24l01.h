#ifndef __LIB_NRF24L01_H__
#define __LIB_NRF24L01_H__

//#define SENDER
//#define RECVER

#ifdef SENDER
#define LCL_ADDR	{0XE7, 0XE7, 0XE7, 0XE7, 0XE7}
#define TAR_ADDR	{0XE7, 0XE7, 0XE7, 0XE7, 0XE6}
#endif

#ifdef RECVER
#define TAR_ADDR	{0XE7, 0XE7, 0XE7, 0XE7, 0XE7}
#define LCL_ADDR	{0XE7, 0XE7, 0XE7, 0XE7, 0XE6}
#endif

#ifndef TAR_ADDR
#define TAR_ADDR {0,0,0,0,0}
#endif

#ifndef LCL_ADDR
#define LCL_ADDR {0,0,0,0,0}
#endif

extern bool send_flag, recv_flag, rety_flag;
void init_nrf24l01(void);
void nrf24l01_send(const char *src, u8 len);
void recv_status(void);
char * nrf24l01_recv(void);

#endif