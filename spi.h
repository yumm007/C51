#ifndef __LIB_SPI_H__
#define __LIB_SPI_H__

#define PAGE_SIZE	256

#include "lib.h"

void spi_init(void);
void spi_deinit(void);
u8 spi_send_byte(u8 val);
u8 spi_recv_byte(void);

//int spi_read(u16 addr_h, u16 addr_l, char *dst, u32 buf_size);
int spi_read(long int addr, char *dst, u32 buf_size);

#endif