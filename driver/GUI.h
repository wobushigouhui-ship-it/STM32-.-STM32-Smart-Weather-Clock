


u16 LCD_BGR2RGB(u16 c);
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);  
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);//1
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);//2
void Gui_DrawFont_Num24x48(u16 x, u16 y, u16 fc, u16 bc, const char *str);//3
void Gui_showimage(const unsigned char *p, uint8_t c, uint8_t k, uint8_t x, uint8_t y);//4
//void Font_Test(void);
void Gui_DrawFont_ASCII8x16(u16 x, u16 y, u16 fc, u16 bc, u8 ch);
void Gui_DrawString_8x16(u16 x, u16 y, u16 fc, u16 bc, u8 *str);//5

