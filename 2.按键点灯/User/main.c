/*****************
* 按键点灯
* DT9025A
* 19/12/13
*****************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"

void GPIO_Configuration() {
    GPIO_InitTypeDef GPIO_InitStructure;

    //GPIOA&B
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    //A
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    //B
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init (GPIOB, &GPIO_InitStructure);
}

int main (void) {//主程序
    RCC_Configuration();
    GPIO_Configuration();
    while (1) {
		//u16 GPIO_ReadInputData
		//u8 GPIO_ReadOutputDataBit
		//u16 GPIO_ReadOutputData
		
        if (GPIO_ReadInputDataBit (GPIOA, GPIO_Pin_0))
            GPIO_ResetBits (GPIOB, GPIO_Pin_0);
        else
            GPIO_SetBits (GPIOB, GPIO_Pin_0);
        if (GPIO_ReadInputDataBit (GPIOA, GPIO_Pin_1))
            GPIO_ResetBits (GPIOB, GPIO_Pin_1);
        else
            GPIO_SetBits (GPIOB, GPIO_Pin_1);
    }
}
