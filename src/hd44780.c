#include "hd44780.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stddef.h>
void Delay()
{
    const int delay_length = 1000;
    int i = 0;
    for (i = 0; i < delay_length; i++);
}
void GPIO_Config()
{
     /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable clock for GPIOD */
    RCC_AHB1PeriphClockCmd(LCD_RCC, ENABLE);
    /* Enable clock for SYSCFG */

    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = D7 | D6 | D5 | D4 | E | RS;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(LCD_GPIO, &GPIO_InitStruct);
}

void WriteHalfByte(uint8_t data)
{
    uint16_t setting_bits = 0;
    GPIO_ResetBits(LCD_GPIO, E);
    Delay();

    if (((data >> 0x00) & 0x01) != 0)
    {
        setting_bits |= D4;
    }

    if (((data >> 0x01) & 0x01) != 0)
    {
         setting_bits |= D5;
    }

    if (((data >> 0x02) & 0x01) != 0)
    {
        setting_bits |= D6;
    }

    if (((data >> 0x03) & 0x01) != 0)
    {
        setting_bits |= D7;
    }
    GPIO_ResetBits(LCD_GPIO,D7 | D6 | D5 | D4);
    GPIO_SetBits(LCD_GPIO, setting_bits);
    Delay();
    GPIO_SetBits(LCD_GPIO,E);
    Delay();
    GPIO_ResetBits(LCD_GPIO,E);
}

void LCD_SendByte(uint8_t data)
{
    WriteHalfByte(data >> 0x04);
    WriteHalfByte(data & 0x0f);
}


void LCD_PrintChar(char c) {
    if(((c>=0x20)&&(c<=0x7F)) || ((c>=0xA0)&&(c<=0xFF))) {
        //check if 'c' is within display boundary
        GPIO_SetBits(LCD_GPIO, RS);
        LCD_SendByte(c);
        GPIO_ResetBits(LCD_GPIO, RS);
    }
}

void LCD_Clear()
{
    LCD_SendByte(0x01);
}

void LCD_SelectLine1()
{
    LCD_SendByte(0x80);
}

void LCD_SelectLine2()
{
    LCD_SendByte(0xC0);
}



void LCD_PrintString(char *str)
{
    uint8_t i=0;

    while(str[i] != '\0') {
        LCD_PrintChar(str[i]);
        i++;
    }
}
void LCD_Config()
{
    GPIO_Config();

    GPIO_Config();
    Delay();
    GPIO_ResetBits(GPIOC,D7 | D6 | D5 | D4 | E | RS);

    //uint8_t commands[] = { 0x03, 0x03, 0x03, 0x02, 0x02, 0x08, 0x00, 0x0f, 0x00, 0x01, 0x00, 0x07 };
    uint8_t commands[] = { 0x03, 0x03, 0x03, 0x02, 0x02, 0x08, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x06 };

    size_t i = 0;
    for( i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
    {
        WriteHalfByte(commands[i]);
    }

}
