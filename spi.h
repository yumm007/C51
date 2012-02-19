#ifndef __LIB_SPI_H__
#define __LIB_SPI_H__

#define PAGE_SIZE	256

void spi_init(void);
void spi_deinit(void);
u8 spi_send_byte(u8 val);
u8 spi_recv_byte(void);

int spi_read(u32 addr, char *dst, u32 buf_size);

#endif