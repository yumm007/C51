#ifndef __LIB_I2C_H_
#define __LIB_I2C_H_
#include "lib.h"

#define ST24C512_ADDR	0xA0
#define INIT_MAGIC_NUM	0xf7

void i2c_init(void);
void I2C_ReadByte(u8 *val, u16 n, u16 Data_addr, u8 Dev_addr);
void I2C_WriteByte(u8 *val, u16 n, u16 Data_addr, u8 Dev_addr);
void I2C_Append_Byte(u8 *val, u16 n, u8 Dev_addr);

#endif