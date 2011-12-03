#include "lib.h"
#include "i2c.h"
#include "usart.h"
#include <STC12C5A60S2.H>
#include <string.h>
#include <intrins.h>

sbit SDA	= P1 ^ 3;
sbit SCL	= P1 ^ 5;

#define _Nop() _nop_() /*定义空指令*/
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
	_Nop();
	SCL = 0;
	_Nop();
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

bool i2c_write(u8 *val, u16 n, u16 addr) {
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

unsigned char * i2c_read(u8 *val, u16 n, u16 addr) {
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

	n -= 1;
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


sbit GREEN 	= P1 ^ 0;
sbit YELLOW	= P1 ^ 1;

void i2c_init(void) {
	char tmp_w[] = "test line 2";
	char tmp_r[] = "aaaa aaaa a";
	u8 i;

	P1M0	= 0xD4;		//将P1.3, P1.5配置成标准IO口模式
	P1M1	= 0x03;

	YELLOW = 0;
	GREEN = 0;

	//i2c_write(&tmp_w[0], sizeof(tmp_w), 0);

	for (i = 0; i < sizeof(tmp_w); i++) 
		;//i2c_read(&tmp_r[i], 1, i);
	//i2c_read(&tmp_r[0], sizeof(tmp_r), 0);

	usart_send(tmp_r, sizeof(tmp_r));

	if ( memcmp(tmp_w, tmp_r, sizeof(tmp_w)) != 0) {
		YELLOW = 1;
		while (1);
	} else {
		GREEN = 1;
		while (1);
	}
	
}