#include <STC12C5A60S2.H>
#include "lcd.h"
#include "lib.h"
#include "font.h"
#include <stdio.h>

#define DL	P0
#define DH	P2

sbit  LRD = P1 ^ 0;
sbit  LWR = P1 ^ 1;
sbit  LRS = P1 ^ 2;
sbit  LCS = P1 ^ 3;
sbit  LRST= P1 ^ 4;

#define send_val_to_bus(x) 	{DH = ((x) >> 8); DL = (x);}
#define recv_val_from_bus()	((DH << 8) | DL)

#define LCD_CS_HIGH	LCS = 1		 //片选
#define LCD_CS_LOW	LCS = 0

#define LCD_RD_HIGH LRD = 1	   	//读控制
#define LCD_RD_LOW  LRD = 0

#define LCD_WR_HIGH LWR = 1	    //写控制
#define LCD_WR_LOW  LWR = 0

#define LCD_DC_HIGH LRS = 1	 //寄存器、显存控制
#define LCD_DC_LOW  LRS = 0	 //低电平表示寄存器

void lcd_lib_reset(void) {

	LRST = 1;
	delay_ms(10);
	LRST = 0;
	delay_ms(1);
	LRST = 1;
	delay_ms(200);
}

static u16 read_lcd_id(u16 reg) {
	u16 val = 0xffff;

	LCD_RD_HIGH;	//RD一直为高

	LCD_DC_HIGH;	//初始为高
	LCD_WR_HIGH;	//初始为高
	LCD_CS_HIGH;	//初始为高

	LCD_DC_LOW;
	LCD_CS_LOW;

	send_val_to_bus(reg); 
	LCD_WR_LOW;
	send_val_to_bus(reg); 


	LCD_WR_HIGH;
	LCD_WR_HIGH;
	LCD_DC_HIGH;

//读
	LCD_WR_HIGH;	//WR一直为高
	LCD_DC_LOW;
	LCD_RD_LOW;
	
	//for (i = 0; i < 0xff; i++)
	//	;
	val = recv_val_from_bus();
	LCD_RD_HIGH;
	LCD_CS_HIGH;
	LCD_DC_HIGH;

	return val;
}

void write_lcddata(unsigned short x){
	LCD_RD_HIGH;	//RD一直为高

	LCD_DC_HIGH;
	LCD_CS_LOW;
	LCD_WR_LOW;
	send_val_to_bus(x); 
	LCD_WR_HIGH;
	LCD_CS_HIGH;
	LCD_DC_HIGH;
}

void write_reg(u16 reg, u16 val)
{
	int i;

	for (i = 0; i < 10; i++) {
		LCD_RD_HIGH;	//RD一直为高
	
		LCD_DC_HIGH;	//初始为高
		LCD_WR_HIGH;	//初始为高
		LCD_CS_HIGH;	//初始为高
		
		LCD_DC_LOW;
		LCD_CS_LOW;
	
		send_val_to_bus(reg);
		LCD_WR_LOW;
	
		send_val_to_bus(reg); 
	
		if (val == NO_DATA)
			goto ret;
	
		LCD_WR_HIGH;
		LCD_WR_HIGH;
	
		LCD_DC_HIGH;
	
		//写第二个字节
		send_val_to_bus(reg);
	
		LCD_WR_LOW;
		LCD_WR_LOW;
	
		send_val_to_bus(val);
	
	ret:
		LCD_WR_HIGH;
		LCD_WR_HIGH;
	
		LCD_CS_HIGH;
	
		LCD_DC_HIGH;
	
		//delay_ms(1);
	}
}

extern void LCD_Clear(u16 Color);
void LCD_Draw_Rectangle(u16 startx, u16 starty, u16 endx, u16 endy);

void LCD_Lib_Initial(void)
{

	lcd_lib_reset();
	printf("read lcd id = 0x%x.\n", (int)read_lcd_id(0x00));		  
	//******************sub  lcd initial***************
	write_reg(0xE5, 0x8000); 

	write_reg(0x0028,0x0006);
	write_reg(0x0007,0x0021);
	write_reg(0x0000,0x0001);// Start internal OSC
	delay_ms(30);
	write_reg(0x0007,0x0023);
	write_reg(0x0010,0x0000); // Sleep mode off
	delay_ms(30);
	write_reg(0x0007,0x0033);// Display

	write_reg(0x0001,0x2b3f);// Driver output control

	write_reg(0x0002,0x0600);//Set to line inversion
	write_reg(0x0011,0x6830);// Entry mode setup
	write_reg(0x0003,0xAAAE);//Step-up factor/cycle setting
	write_reg(0x000F,0x0000);// Gate scan position start at G0
	write_reg(0x000E,0x3200);// Set alternating amplitude of VCOM
	write_reg(0x001E,0x00A8);// Set VcomH voltage A8 100614
	write_reg(0x000C,0x0005);// Adjust VCIX2 output voltage
	write_reg(0x000D,0x0009);// Set amplitude magnification of VLCD63
	write_reg(0x000B,0x5308);//Frame cycle control,don't change for crosstalk//0x5375
	
	// ----------- Special command ----------//
	write_reg(0x0025,0xC000);//Frame freq control
	write_reg(0x003f,0xBB84);// System setting,internal command
	write_reg(0x0027,0x0567);// Internal Vcomh/Vcoml timing
	write_reg(0x0020,0x316C);// Internal VCOM strength
	write_reg(0x0012,0x06D9);// Internal VCOM strength
	write_reg(0x003b,0x0002);// Set gamma
			
	write_reg(0x0020,0xB0EB);//internal vcom strength //internal register
	write_reg(0x0022, NO_DATA);

	//printf("init cmp.\n");
	//while (1) {
		//LCD_Clear(Green);
		//LCD_Clear(Red);
		LCD_Clear(Blue);
	//}
	//printf("screen clear.\n");
	//LCD_Draw_Rectangle(10, 20, 40, 60);
}

 void MainLCD_Window_Set(unsigned short sax, unsigned short say, unsigned short eax, unsigned short eay)
 { 	 
	  write_reg(0x44, (eax << 8) | sax);
	  write_reg(0x45, say);
	  write_reg(0x46, eay);
	  write_reg(0x4e, sax);
	  write_reg(0x4f, say);
	  write_reg(0x22, NO_DATA);    
 }


#define START_WRITE_LCD {LCD_RD_HIGH; LCD_DC_HIGH; LCD_CS_LOW;}
#define SEND_NEXT_LCD(x) {LCD_WR_LOW;LCD_WR_HIGH;}
#define END_WRITE_LCD {LCD_CS_HIGH; LCD_DC_HIGH;}

void LCD_Clear(u16 Color){
    int i, j;
    MainLCD_Window_Set(0,0,SCREEN_ROW-1,SCREEN_LINE-1);
	START_WRITE_LCD;
	send_val_to_bus(Color);
    for(j = 0; j < SCREEN_ROW; j++)
	 	for(i = 0; i < SCREEN_LINE; i++)
			SEND_NEXT_LCD(Color);	
   END_WRITE_LCD;
}

void lcd_print(FONT_SIZE_T size, int row, int lines, const unsigned char *str) {
    unsigned char is_hz;
    unsigned char bit_buf[FONT_MAX * (FONT_MAX/8)];
    int i, j, p;
    FONT_TYPE_T font_type;

	unsigned short color[] = {Black, White};

    while (*str != '\0') {
		is_hz = (*str) > 0xa0 ? 1 : 0;	//判断是否为汉字	
		//返回字体类型
		font_type = get_word_type(size, is_hz);
		//设置屏幕输出的起始位置
		set_lcd_row_line(font_type, &row, &lines);

		//从字库中取出当前字的点阵, 并返回总共的字节数
		j = get_bitmap(font_type, bit_buf, str);
		START_WRITE_LCD;
		for (i = 0; i < j; i++) { 
		    //从低位到高位，位为0表示描背景色，位为1表示为字体颜色；
		    //send_8bit(font_type, bit_buf[i]);
			//for (p = 7; p >= 0; p--)
				//{LCD_WR_LOW;send_val_to_bus( bit_buf[i] & (1 << p) ? Black : White);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 7) & 1 ]);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 6) & 1 ]);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 5) & 1 ]);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 4) & 1 ]);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 3) & 1 ]);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 2) & 1 ]);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 1) & 1 ]);LCD_WR_HIGH;}
		   {LCD_WR_LOW;send_val_to_bus( color[ (bit_buf[i] >> 0) & 1 ]);LCD_WR_HIGH;}
		}
		END_WRITE_LCD;
		//row, line始终指向下一个空白位置,可能换行也可能跳到行首
		str = is_hz ? str + 2 : str + 1;	//指向下一个字符
    }
}

int print_test(void) {
    unsigned char tmp[] = {0xCE, 0xD2, 0xC3, 0xC7, 0x00 }; //汉字GB2312 "我们"
    //lcd_print(FONT_12, 0, 0, "ABCDE");
    //lcd_print(FONT_16, 0, 0, "abced");
    //lcd_print(FONT_24, 0, 0, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    //lcd_print(FONT_16, 0, 0, "啊爸爸把差");
    lcd_print(FONT_16, 0, 0, "Hello World!我们永远不分开永远在一起我们永远在一起永远不分开永远在一起我们永远在一起永远不分开永远在一起我们永远在一起永远不分开永远在一起我们永远在一起永远不分开永远在一起我们永远在一起永远不分开永远在一起我们永远在一起永远不分开永远在一起我们永远在一起永远不分");
	//printf("max val = %f.\n", 65534.0+100.0);
    return 0;
}
