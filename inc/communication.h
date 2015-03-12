
/**
  ******************************************************************************
  * @file    ESP01_communication.h
  * @author
  * @version V0.0.1
  * @date    28-October-2011
  * @brief   This file contains all the functions prototypes for the stm32f4_discovery_lis302dl.c
  *          firmware driver.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMMUNICATION_H_INCLUDED
#define COMMUNICATION_H_INCLUDED
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
/**
  * @brief  ESP USART Interface pins
  */

#define SSID "Piski"
#define PASSWORD "meaculpa_T34"
//Clock settings
#define GPIO_RCC_ClockCmd RCC_AHB1PeriphClockCmd
#define USART_RCC_ClockCmd RCC_APB2PeriphClockCmd
#define GPIO_RCC RCC_AHB1Periph_GPIOA
#define USART_RCC RCC_APB2Periph_USART1



//Pin settings
#define USART_TX_PIN GPIO_Pin_9
#define USART_RX_PIN GPIO_Pin_10
#define USART_GPIO_AF GPIO_AF_USART1
#define USART_TX_PinSource GPIO_PinSource9
#define USART_RX_PinSource GPIO_PinSource10
#define GPIO_BASE GPIOA
#define USART USART1
//IRQ settings
#define USART_IRQn USART1_IRQn
#define USART_IRQ_Handler() USART1_IRQHandler()

typedef enum AT_RESULT {OK, ERR} AT_RESULT;

/******************************************************************************/
/************************ Start AT control command set  ***********************/
/******************************************************************************/


void USART_Config();
void USART_SendChar(char ch);
void USART_SendString(char *str);
AT_RESULT ESP_SetCommand(char command[]);
AT_RESULT ESP_GetCommand(char command[],char answer[]);
AT_RESULT ESP_TCP_GetIP(char ip[]);
AT_RESULT ESP_TCP_StartServer();
bool proceedSocketData();

#endif /* COMMUNICATION_H_INCLUDED */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


