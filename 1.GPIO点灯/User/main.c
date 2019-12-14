/*****************
* GPIO点灯
* DT9025A
* 19/12/13
*****************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"

void GPIO_Configuration() {
    GPIO_InitTypeDef GPIO_InitStructure;

    //GPIOB
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOB, &GPIO_InitStructure);
}

int main (void) {//主程序
    RCC_Configuration();
    GPIO_Configuration();
    while (1) {
        GPIO_Write (GPIOB, 0x0001);
        delay_ms (100);
        GPIO_Write (GPIOB, 0x0000);
        delay_ms (100);

        GPIO_SetBits (GPIOB, GPIO_Pin_0);
        delay_ms (100);
        GPIO_ResetBits (GPIOB, GPIO_Pin_0);
        delay_ms (100);

        GPIO_WriteBit (GPIOB, GPIO_Pin_0, Bit_SET);
        delay_ms (100);
        GPIO_WriteBit (GPIOB, GPIO_Pin_0, Bit_RESET);
        delay_ms (100);
    }
}
