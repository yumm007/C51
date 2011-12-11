#ifndef __LIB_LED_H__
#define __LIB_LED_H__

void led_init(void);
void led_print(char *str);
void set_xy(u8 x, u8 y);
void led_clr(void);

#endif