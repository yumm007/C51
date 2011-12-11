#include "lib.h"
#include "i2c.h"
#include "usart.h"
#include <STC12C5A60S2.H>
#include <string.h>
#include <intrins.h>
#include <stdio.h>

sbit SDA	= P0 ^ 1;
sbit SCL	= P0 ^ 0;

#define _Nop() _nop_() /*∂®“Âø’÷∏¡Ó*/
#define I2C_DELAY delay_ms(1)

static bool i2c_free(void) {
	SCL = 1;
	SDA = 1;
	I2C_DELAY;
	I2C_DELAY;
	return (SCL && SDA);
}

static void i2c_start(void) {
	SDA = 1;
	SCL = 1;
	I2C_DELAY;
	SDA = 0;
	I2C_DELAY;
	SCL = 0;
	I2C_DELAY;
}

static void i2c_stop(void) {
	SCL = 0;
	I2C_DELAY;
	SDA = 0;
	I2C_DELAY;
	SCL = 1;
	I2C_DELAY;
	SDA = 1;
	I2C_DELAY;
}

static void i2c_sendack(void) {
	SCL = 0;
	_Nop();
	SDA = 0;
	_Nop();
	SCL = 1;
	_Nop();
	_Nop();
	_Nop();
	_Nop();
	_Nop();
	SCL = 0;
}

static void i2c_waitack(void) {
	SCL = 0;
	_Nop();
	SDA = 1;
	_Nop();
	SCL = 1;
	_Nop();
	while (SDA)
		;
	SCL = 0;
	_Nop();
}

static void i2c_sendnack(void) {
	SCL = 0;
	_Nop();
	SDA = 1;
	_Nop();
	SCL = 1;
	_Nop();
	SCL = 0;
	_Nop();
}

static void i2c_sendbit(bool v) {
	SCL = 0;
	_Nop();
	SDA = v;
	_Nop();
	SCL = 1;
	_Nop();
	SCL = 0;
	_Nop();
}

static bool i2c_recvbit(void) {
	bool v;
	SCL = 0;
	_Nop();
	SCL = 1;
	_Nop();
	v = SDA;
	SCL = 0;
	_Nop();
	return v;
}

static void i2c_sendbyte(u8 v) {
	u8 i;
	for (i = 0; i < 8; i++, v <<= 1)
		i2c_sendbit(v & 0x80);	
}

static u8 i2c_recvbyte(void) {
	u8 i, ret = 0;
	for (i = 0; i < 8; i++)
		ret |= (i2c_recvbit() << (7 - i));
	return ret;
} 

bool i2c_write(u8 *val, u32 n, u16 addr) {
	if (!i2c_free())
		return FALSE;

	i2c_start();
	i2c_sendbyte(ST24C512_ADDR & 0xfe);
	i2c_waitack();

	i2c_sendbyte(addr >> 8);
	i2c_waitack();
	i2c_sendbyte(addr & 0xff);
	i2c_waitack();

	while (n--) {
		i2c_sendbyte(*val);
		i2c_waitack();
		val++;
	}
	i2c_stop();

	return TRUE;
}

unsigned char * i2c_read(u8 *val, u32 n, u16 addr) {
	if (!i2c_free())
		return NULL;

	i2c_start();
	i2c_sendbyte(ST24C512_ADDR & 0xfe);
	i2c_waitack();

	i2c_sendbyte(addr >> 8);
	i2c_waitack();
	i2c_sendbyte(addr & 0xff);
	i2c_waitack();
	
	i2c_start();
	i2c_sendbyte(ST24C512_ADDR | 1);
	i2c_waitack();

	n--;
	while (n--) {
		*val = i2c_recvbyte();
		i2c_sendack();
		val++;
	}

	*val = i2c_recvbyte();
	i2c_sendnack();

	i2c_stop();

	return val;
}


void i2c_init(void) {
	char tmp_w[] = "i2c init COMP";
	char tmp_r[] = "aaaa aaaa aaa";
	u8 i;
	
	i2c_write(&tmp_w[0], sizeof(tmp_w), 0);
	//for (i = 0; i < sizeof(tmp_w); i++) 
	//	i2c_write(&tmp_w[i], 1, i);
	//delay_ms(10);
	for (i = 0; i < sizeof(tmp_w); i++) 
		i2c_read(&tmp_r[i], 1, i);
	//i2c_read(&tmp_r[0], sizeof(tmp_r), 0);

	printf("i2c:%s\n", tmp_r);

}