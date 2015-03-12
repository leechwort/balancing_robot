/**
  ******************************************************************************
  * @file    stm32f4_discovery_lis302dl.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    28-October-2011
  * @brief   This file provides a set of functions needed to manage the LIS302DL
  *          MEMS accelerometer available on STM32F4-Discovery Kit.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/

#include <string.h>

#include "communication.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "hd44780.h"


/* AT Commands ---------------------------------------------------------------*/

char usart_buffer[500];
bool command_complete = false;
AT_RESULT current_result;
uint8_t usart_buffer_position = 0;
// PID regulator coefficients
float P = 0;
float I = 0;
float D = 0;

// Is engine running
bool is_running = false;

// PWM value
int pwm = 0;



void Delay_communication()
{
     for (int i = 0; i < 1000000; i++);
}


/**
  * @brief  Set USART Initialization.
  * @param  None
  * @retval None
  */
void USART_Config()
{

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    GPIO_RCC_ClockCmd(GPIO_RCC, ENABLE);
    USART_RCC_ClockCmd(USART_RCC, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Pin = USART_TX_PIN | USART_RX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIO_BASE, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIO_BASE, USART_TX_PinSource, USART_GPIO_AF);
    GPIO_PinAFConfig(GPIO_BASE, USART_RX_PinSource, USART_GPIO_AF);

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 9600;
    USART_Init(USART, &USART_InitStructure);

    NVIC_EnableIRQ(USART_IRQn);
    USART_Cmd(USART, ENABLE);
    USART_SendString("ATE0\r\n");//Disable echo for ESP device

    USART_ITConfig(USART, USART_IT_RXNE, ENABLE);
}

void USART_SendChar(char ch)
{
    USART_SendData(USART, ch);
}

void USART_SendString(char *str)
{
    uint8_t i=0;

    while(str[i] != '\0') {
        while (USART_GetFlagStatus(USART, USART_FLAG_TXE) != SET);
        char sending_char = str[i];
        USART_SendChar(sending_char);
        i++;
    }
}

AT_RESULT ESP_SetCommand(char command[])
{
    USART_SendString(command);
    command_complete = false;
    while (command_complete != true)
    {
        int m = 0;
    }
    // Cleaning usart_buffer and reset buffer position
    usart_buffer_position = 0;
    memset(usart_buffer,0,sizeof(usart_buffer));
    return current_result;
}

AT_RESULT ESP_GetCommand(char command[],char answer[])
{
    //result = NULL;
    USART_SendString(command);
    command_complete = false;
    while (command_complete != true);
    if (current_result == OK)
    {
        char *terminator;
        terminator = strstr(usart_buffer,"\r\n\r\n");
        *terminator = '\0';
    }
    memset(answer,0, sizeof(answer));
    strncpy(answer,usart_buffer,sizeof(answer)-1);
    usart_buffer_position = 0;
    memset(usart_buffer,0,sizeof(usart_buffer));
    return current_result;
}

AT_RESULT ESP_TCP_GetIP(char ip[])
{
    char result_ip[] = "000.000.000.000";

    USART_SendString("AT+CIFSR\r\n");
    command_complete = false;
    while (command_complete != true);
    if (current_result == OK)
    {
        uint16_t input_length = 0;
        uint16_t result_length = 0;
        uint16_t result_position = 0;
        uint16_t buffer_pos = 0;
        input_length = strlen(usart_buffer);
        for (buffer_pos = 0; buffer_pos < input_length; buffer_pos++)
        {
            char current_char;
            current_char = usart_buffer[buffer_pos];
            // Format of ip addr is 123.456.789.123
            if ((isdigit(current_char)) || (current_char == '.'))
            {
                result_ip[result_position++] = current_char;
            }
        }
        result_ip[result_position++] = '\0';
    }

    strcpy(ip,result_ip);
    usart_buffer_position = 0;
    memset(usart_buffer,0,sizeof(usart_buffer));
    return current_result;
}

AT_RESULT ESP_TCP_StartServer()
{
    AT_RESULT at_res = ERR;

    at_res = ESP_SetCommand("AT\r\n");
    if (at_res == OK)
    {
        LCD_Clear();
        LCD_PrintString("AT OK!!!");
        Delay_communication();
    }
    else
    {
        LCD_Clear();
        LCD_PrintString("AT ERR!!!");
        return ERR;
    }


    at_res = ESP_SetCommand("AT+CWQAP\r\n");
    if (at_res == OK)
    {
        LCD_Clear();
        LCD_PrintString("Disconnect OK!!!");
        Delay_communication();
    }
    else
    {
        LCD_Clear();
        LCD_PrintString("Disconnect ERR!!!");
        return ERR;
    }

    at_res = ESP_SetCommand("AT+CWMODE=1\r\n");
    if (at_res == OK)
    {
        LCD_Clear();
        LCD_PrintString("CWMODE OK!!!");
        Delay_communication();
    }
    else
    {
        LCD_Clear();
        LCD_PrintString("CWMODE ERR!!!");
        return ERR;
    }

    at_res = ESP_SetCommand("AT+CIPMODE=0\r\n");
    if (at_res == OK)
    {
        LCD_Clear();
        LCD_PrintString("CIPMODE OK!!!");
        Delay_communication();
    }
    else
    {
        LCD_Clear();
        LCD_PrintString("CIPMODE ERR!!!");
        return ERR;
    }
    for (int attempts = 0; attempts < 5; attempts++)
    {
        at_res = ESP_SetCommand("AT+CWJAP=\"Piski\",\"meaculpa_T34\"\r\n");
        if (at_res == OK)
        {
            break;
        }
        Delay_communication();
    }

    if (at_res == OK)
    {
        LCD_Clear();
        LCD_PrintString("Connection OK!");
        Delay_communication();
    }
    else
    {
        LCD_Clear();
        LCD_PrintString("Connection ERR!");
        return ERR;
    }

    Delay_communication();
    char ip_addr[20];
    const int max_attempts = 10;
    for (int attempts = 0; attempts < max_attempts; attempts++)
    {

        at_res = ESP_TCP_GetIP(ip_addr);
        LCD_SelectLine1();
        LCD_Clear();
        LCD_PrintString(ip_addr);
        if (at_res == OK)
            break;
        Delay_communication();
    }
    at_res = ESP_SetCommand("AT+CIPMUX=1\r\n");
    if (at_res == OK)
    {
        LCD_Clear();
        LCD_PrintString("CIPMUX OK!!!");
        Delay_communication();
    }
    else
    {
        LCD_Clear();
        LCD_PrintString("CIPMUX ERR!!!");
        return ERR;
    }



    Delay_communication();
    for (int attempts = 0; attempts < max_attempts; attempts++)
    {

        at_res = ESP_TCP_GetIP(ip_addr);
        LCD_SelectLine1();
        LCD_Clear();
        LCD_PrintString(ip_addr);
        if (at_res == OK)
            break;
        Delay_communication();
    }

    LCD_SelectLine2();
    at_res = ESP_SetCommand("AT+CIPSERVER=1,8888\r\n");
    if (at_res == OK)
    {
        LCD_PrintString("TCP OK!");
        Delay_communication();
    }
    else
    {
        LCD_PrintString("TCP ERR!");
        return ERR;
    }

}




/* Private function ----------------------------------------------------------*/
/**
  * @brief  Handling received from PC data
  * @param  None
  * @retval None
  */
void processReceivedData(uint16_t data)
{
    char OK_str[] = "OK\r\n";
    char ERROR_str[] = "ERROR\r\n";
    char NO_CHANGE_str[] = "no change\r\n";
    char NEW_DATA_str[] = "IPD";
    usart_buffer[usart_buffer_position++] = data;
    if (data == '\n')
    {
        //Find, if string ends with OK
        char *is_ok;
        char *is_error;
        char *is_no_change;
        char *is_new_data;
        is_ok = strstr(usart_buffer,OK_str);
        is_error = strstr(usart_buffer,ERROR_str);
        is_no_change = strstr(usart_buffer,ERROR_str);
        is_new_data = strstr(usart_buffer,NEW_DATA_str);
        if ((is_ok != NULL) || (is_no_change != NULL))
        {
            current_result = OK;
        }
        if (is_error != NULL)
        {
            current_result = ERR;
        }
        if (is_new_data != NULL)
        {
            proceedSocketData();
            current_result = OK;
        }
        command_complete = true;
    }
}

bool proceedSocketData()
{

    //COMMANDS
    char START_COM[] = "START";
    char STOP_COM[] = "STOP";
    char SET_PID[] = "SET_PID:";
    char SET_PWM[] = "SET_PWM:";



    const uint8_t data_offset = 7;
    char NEW_DATA_str[] = "+IPD,";
    char NEW_DATA_DELIMITER[] = ":";
    char data_string[100] = {0};
    char *new_data_pointer;
    if (strstr(usart_buffer,NEW_DATA_str) == NULL)
    {
        return false;
    }

    new_data_pointer = strstr(usart_buffer,NEW_DATA_DELIMITER);
    if (new_data_pointer == NULL)
    {
        return false;
    }
    data_string[0] = new_data_pointer + 1;
    for (int i = 0; i < 20; i++)
    {
        data_string[i] =*(new_data_pointer + i + 1);
        if (data_string[i] == '\r')
        {
            data_string[i] = '\0';
            memset(usart_buffer,0,sizeof(usart_buffer));
            usart_buffer_position = 0;
            break;
        }
    }
    memset(usart_buffer,0,sizeof(usart_buffer));
    usart_buffer_position = 0;

    // Start parsing on commands
    // Check start-stop
    if (strstr(data_string, START_COM) != NULL)
    {
        is_running = true;
    }

    if (strstr(data_string, STOP_COM) != NULL)
    {
        is_running = false;
    }

    if (strstr(data_string, SET_PID) != NULL)
    {
        double value = 12.34;
        sscanf(data_string,"SET_PID:%e",&value);
        //sscanf(data_string,"SET_PID:%d",&value);
        int j = 0;
    }

    return true;
}


void USART_IRQ_Handler()
{
    if (USART_GetITStatus(USART, USART_IT_RXNE)!= RESET)
    {
        uint16_t receivedData = 0;
        while (USART_GetFlagStatus(USART, USART_FLAG_RXNE) != RESET)
        {
            receivedData = USART_ReceiveData(USART);
            processReceivedData(receivedData);
        }
        USART_ClearITPendingBit(USART,USART_IT_RXNE);
    }
}
