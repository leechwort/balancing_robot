/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/


#include "stm32f4xx_conf.h"
#include "hd44780.h"
#include "communication.h"


/*******************************************************************/


int main(void)
{
    char result_buffer[50];
    AT_RESULT at_res = ERR;


    LCD_Config();
    LCD_Clear();



    USART_Config();

    at_res = ESP_TCP_StartServer();

    //proceedSocketData();


    while(1)
    {
        for (int i = 0; i < 100000; i++);




        int j = 0;
    }
}



