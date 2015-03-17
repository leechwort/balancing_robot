/**
  ******************************************************************************
  * @file    motor_control.h
  * @author  Artem Synytsyn
  * @version V1.0.0
  * @date    22 october 2014
  * @brief   This library contains motor control function
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif


#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

/**
* @brief  L298N motor driver interface pins
*/
#define MOTOR_IN_1_PIN 		GPIO_Pin_5
#define MOTOR_IN_2_PIN 		GPIO_Pin_12
#define MOTOR_IN_1_SOURCE 	GPIO_PinSource5
#define MOTOR_IN_2_SOURCE 	GPIO_PinSource12
#define MOTOR_IN_PORT 		GPIOA
#define MOTOR_IN_CLK 		RCC_AHB1Periph_GPIOA

#define PWM_TIMER			TIM3
#define PWM_PIN 			GPIO_Pin_5
#define PWM_SOURCE 			GPIO_PinSource5
#define PWM_PORT 			GPIOB
#define PWM_CLK 			RCC_AHB1Periph_GPIOB
#define PWM_TIM_CLK 		RCC_APB1Periph_TIM3
#define PWM_TIM_AF_CFG 		GPIO_AF_TIM3             /* TIM3 */

/**
* @brief  PWM configuration
*/
#define TIM_COUNTER_CLOCK 80000
#define TIM_PERIOD 512
#define FAST_MOTOR_STOP_POWER 0.5

/** @defgroup Exported functions
* @{
*/
void MOTOR_Config(void);
void MOTOR_SetPower(int16_t value);
void MOTOR_Off(void);


#ifdef __cplusplus
}
#endif

#endif /*__MOTOR_CONTROL_H */
