#ifndef __LIB_NRF24L01_H__
#define __LIB_NRF24L01_H__

#define TAR_ADDR	{0XE7, 0XE7, 0XE7, 0XE7, 0XE7}

extern bool send_flag, recv_flag, rety_flag;
void init_nrf24l01(void);
void nrf24l01_send(const char *src, u8 len);

#endif