#ifndef __LIB_LCD_H__
#define __LIB_LCD_H__

#define NO_DATA 	0xffff
#define SCREEN_ROW	240
#define SCREEN_LINE 320

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

void LCD_Lib_Initial(void);
void write_lcddata(unsigned short x);

void start_write_lcd(void);
void send_next_lcd(unsigned short x);
void end_write_lcd(void);

#endif