/*****************
* 一键闪灯
* DT9025A
* 19/12/24
*****************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"

uint8_t mode = 0;
uint8_t GPIO_MSG = GPIO_Pin_0;
uint32_t DELAY_LOOP = 100;

void GPIO_Configuration() {
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    //GPIOA
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);

    //NVIC
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);

    //EXTI
    GPIO_EXTILineConfig (GPIO_PortSourceGPIOA, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init (&EXTI_InitStructure);

    //GPIOB
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOB, &GPIO_InitStructure);
}

void EXTI0_IRQHandler() {
    if (EXTI_GetITStatus (EXTI_Line0) != RESET) {
        DELAY_LOOP = 0;
        if (mode < 3)
            mode++;
        else
            mode = 0;
        while (GPIO_ReadInputDataBit (GPIOA, GPIO_Pin_0));
        EXTI_ClearITPendingBit (EXTI_Line0);
    }
}

int main (void) {   //主程序
    uint16_t last = 0;
    short dir = 1;
    u8 us = 0;

    RCC_Configuration();
    GPIO_Configuration();

    while (1) {
        if (DELAY_LOOP == 0) {
            if (GPIO_MSG & 1)
                GPIO_MSG = GPIO_Pin_1;
            else
                GPIO_MSG = GPIO_Pin_0;
            switch (mode) {
                case 0:
                    us = 0;
                    DELAY_LOOP = 1000;
                    break;
                case 1:
                    us = 0;
                    DELAY_LOOP = 2000;
                    break;
                case 2:
                    us = 0;
                    DELAY_LOOP = 100;
                    break;
                case 3:
                    us = 1;
                    DELAY_LOOP = (last += dir);
                    if (DELAY_LOOP == 9900)
                        dir = -100;
                    else if (DELAY_LOOP == 100)
                        dir = 100;
                    break;
            }
        }
        DELAY_LOOP -= 1;
        GPIO_Write (GPIOB, GPIO_MSG);
        if (us)
            delay_us (1);
        else
            delay_ms (1);
    }
}
