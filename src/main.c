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
#include "motor_control.h"
#include "MPU6050.h"

int16_t ax,ay,az; // Accelerometer data
int16_t rx,ry,rz; // Gyroscope data
int32_t counter = 0;

struct AccGyroData
{
    int16_t ax,ay,az; // Accelerometer values
    int16_t rx,ry,rz; // Gyroscope values
} mpu_data;
/*******************************************************************/

void MPU6050_ReadGyro(int16_t* gyroBuff)
{
    u8 tmpBuffer[6];
    MPU6050_I2C_BufferRead(MPU6050_DEFAULT_ADDRESS, tmpBuffer, MPU6050_RA_GYRO_XOUT_H, 6);
    for (int i = 0; i < 3; i++)
    {
        gyroBuff[i] = ((int16_t) ((uint16_t) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
    }
}
void MPU6050_ReadAcc(int16_t* accBuff)
{
    u8 tmpBuffer[6];
    MPU6050_I2C_BufferRead(MPU6050_DEFAULT_ADDRESS, tmpBuffer, MPU6050_RA_ACCEL_XOUT_H, 6);
    for (int i = 0; i < 3; i++)
    {
        accBuff[i] = ((int16_t) ((uint16_t) tmpBuffer[2 * i] << 8) + tmpBuffer[2 * i + 1]);
    }
}

void INT_GPIO_Config(void) {

    /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable clock for GPIOD */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Tell system that you will use PD0 for EXTI_Line0 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

    EXTI_InitStruct.EXTI_Line = EXTI_Line13;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);

    EXTI_ClearITPendingBit(EXTI_Line13);
}

void MPU_6050_Config()
{
    uint8_t bit = 0;
    MPU_I2C_ClockToggling();
    //for (int i = 0; i < 100000;i++);
    MPU6050_I2C_Init();
    //MPU6050_SetClockSource(7);
    //for (int i = 0; i < 100000;i++);

    /*MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG,MPU6050_INTCFG_INT_LEVEL_BIT, &bit);
    MPU6050_FullReset();*/
    MPU6050_Initialize();

    MPU6050_WriteBit(MPU6050_DEFAULT_ADDRESS, 0x38, 0, 1);
    MPU6050_WriteBit(MPU6050_DEFAULT_ADDRESS, 0x3A, 0, 1);




    uint8_t prescaler = 9; // 800 Hz speed
    MPU6050_I2C_ByteWrite(MPU6050_DEFAULT_ADDRESS, &prescaler, MPU6050_RA_SMPLRT_DIV);



    MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_INT_PIN_CFG,MPU6050_INTCFG_INT_LEVEL_BIT, &bit);


}
int main(void)
{
    char result_buffer[50];
    AT_RESULT at_res = ERR;


	MPU_6050_Config();
	INT_GPIO_Config();



    //LCD_Config();
    //LCD_Clear();



    USART_Config();

    //at_res = ESP_TCP_StartServer();
    MOTOR_Config();
    MOTOR_SetPower(300);
    int counter = 0;
    while(1)
    {

        /*int16_t buffer[6];
    	MPU6050_GetRawAccelGyro(&buffer);
    	ax = buffer[0];
    	ay = buffer[1];
    	az = buffer[2];

    	rx = buffer[3];
    	ry = buffer[4];
    	rz = buffer[5];*/
    	for (int j = 0; j < 10000; j++);
    	MOTOR_SetPower(counter++);
    	if (counter > 512)
        {
            counter  = -512;
        }
    }
}


void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {

        static i = 0;
        i++;
        if (i >= 1000)
        {
            counter++;
            i = 0;
        }

        int m = 0;
        int16_t gyro_buffer[3];
        int16_t acc_buffer[3];
        //


        MPU6050_ReadGyro(&gyro_buffer);
        MPU6050_ReadAcc(&acc_buffer);

        mpu_data.ax = gyro_buffer[0];
        mpu_data.ay = gyro_buffer[1];
        mpu_data.az = gyro_buffer[2];

        mpu_data.rx = acc_buffer[0];
        mpu_data.ry = acc_buffer[1];
        mpu_data.rz = acc_buffer[2];


        uint8_t bitdata;
        MPU6050_ReadBit(MPU6050_DEFAULT_ADDRESS,MPU6050_RA_INT_STATUS,0,&bitdata);
        EXTI_ClearITPendingBit(EXTI_Line13);

    }
}
