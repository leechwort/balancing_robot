/**
  ******************************************************************************
  * @file    motor_control.c
  * @author  Artem Synytsyn
  * @version V1.0.0
  * @date    22 october 2014
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the dc motor:
  *           - PWM speed regulation
  *           - Control dc motor via L298N driver
  *           - Measure dc motor via optical encoder
  *
  *  @verbatim
  *
  *          ===================================================================
  *                                 How to use this driver
  *          ===================================================================
  *           1. Configure PWM using PWM_Config() function
  *
  *           2. Configure L298N driver inputs using L298_InputsConfig() function
  *
  *           3. Control DC your motor using this way:
  *                - Direction and power is controlled by
  *                			MOTOR_SetPower(int16_t value) function:
  *                			value can vary from TIM_PERIOD to -TIM_PERIOD
  *
  *                - Zero value in parameter MOTOR_SetPower() function
  *                			means fast stop mode, power of fast stop mode
  *                			controlled by FAST_MOTOR_STOP_POWER coefficient. It
  *                			can vary from 1(maximal power) to 0(no fast stop mode)
  *
  *                - MOTOR_Off() function fully stops DC motor power
  *
  *
  ******************************************************************************
  */


#include "motor_control.h"

/* Private function prototypes -----------------------------------------------*/
void L298_InputsConfig(void);
void MOTOR_Config(void);


/**
 *  @brief   Initialization and Configuration
 *
@verbatim
 ===============================================================================
                        Initialization and Configuration
 ===============================================================================

@endverbatim
  * @{
  */


/**
  * @brief  Configure input channel for IN1(PB4) and IN2(PB7) driver L298
  * @param  None
  * @retval None
  */
void L298_InputsConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(MOTOR_IN_CLK, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = MOTOR_IN_1_PIN | MOTOR_IN_2_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(MOTOR_IN_PORT, &GPIO_InitStructure);
}


/**
  * @brief  Configure Channel 1 of TIM4 PWM for PB6 pin.
  * @param  None
  * @retval None
  */
void PWM_Config(void)
{
	  uint16_t PrescalerValue = 0;
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  TIM_OCInitTypeDef  TIM_OCInitStructure;

	  GPIO_InitTypeDef GPIO_InitStructure;

	  /* TIM4 clock enable */
	  RCC_APB1PeriphClockCmd(PWM_TIM_CLK, ENABLE);

	  /* GPIOC clock enable */
	  RCC_AHB1PeriphClockCmd(PWM_CLK, ENABLE);

	  /* GPIOC Configuration: TIM4 CH1 (PB6) */
	  GPIO_InitStructure.GPIO_Pin = PWM_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	  GPIO_Init(PWM_PORT, &GPIO_InitStructure);

	  /* Connect TIM4 pin to AF2 */

	  GPIO_PinAFConfig(PWM_PORT, PWM_SOURCE, PWM_TIM_AF_CFG);

	  /* Compute the prescaler value */
	  PrescalerValue = (uint16_t) ((SystemCoreClock /2) / TIM_COUNTER_CLOCK) - 1;

	  /* Time base configuration */
	  TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD;
	  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	  TIM_TimeBaseInit(PWM_TIMER, &TIM_TimeBaseStructure);

	  /* PWM1 Mode configuration: Channel1 */
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_Pulse = 0;
	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	  TIM_OC2Init(PWM_TIMER, &TIM_OCInitStructure);

	  TIM_OC2PreloadConfig(PWM_TIMER, TIM_OCPreload_Enable);

	  /* TIM3 enable counter */
	  TIM_Cmd(PWM_TIMER, ENABLE);
}

/**
  * @brief  Configure PWM and Input control for DC motor
  * @param  None
  * @retval None
  */
void MOTOR_Config(void)
{
	PWM_Config();
	L298_InputsConfig();
}


/**
  * @brief  Set power for DC motor
  * @param  Value of power can vary from -TIM_PERIOD to +TIM_PERIOD
  * @retval None
  */
void MOTOR_SetPower(int16_t value)
{
	if (value > 0)
	{
		GPIO_ResetBits(MOTOR_IN_PORT,MOTOR_IN_1_PIN);
		GPIO_SetBits(MOTOR_IN_PORT,MOTOR_IN_2_PIN);
		TIM_SetCompare2(PWM_TIMER, value);
	}
	else if (value < 0)
	{
		GPIO_ResetBits(MOTOR_IN_PORT,MOTOR_IN_2_PIN);
		GPIO_SetBits(MOTOR_IN_PORT,MOTOR_IN_1_PIN);
		TIM_SetCompare2(PWM_TIMER, -1*value);
	} else if (value == 0)
	{
		GPIO_SetBits(MOTOR_IN_PORT,MOTOR_IN_2_PIN);
		GPIO_SetBits(MOTOR_IN_PORT,MOTOR_IN_1_PIN);
		TIM_SetCompare2(PWM_TIMER, FAST_MOTOR_STOP_POWER*TIM_PERIOD);
	}

}

void MOTOR_Off(void)
{
	TIM_SetCompare2(PWM_TIMER, 0);
}
