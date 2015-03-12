#ifndef HD44780_H_INCLUDED
#define HD44780_H_INCLUDED

#define D7 GPIO_Pin_9
#define D6 GPIO_Pin_12
#define D5 GPIO_Pin_11
#define D4 GPIO_Pin_10

#define E GPIO_Pin_8
#define RS GPIO_Pin_6

#define LCD_GPIO GPIOC
#define LCD_RCC RCC_AHB1Periph_GPIOC

void LCD_Config();
void LCD_PrintChar(char c);
void LCD_Clear();
void LCD_SelectLine1();
void LCD_SelectLine2();
void LCD_PrintString(char *str);


#endif /* HD44780_H_INCLUDED */
