/*****************
* TM1640
* DT9025A
* 20/1/15
*****************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include <stdarg.h>

#define SCLK GPIO_Pin_11
#define DIN GPIO_Pin_12


const u8 buff[21] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef, 0x00};
//---------------      0    1      2    3      4    5      6    7      8     9    0.    1.   2.     3.   4.     5.   6.     7.   8.    9.

void GPIO_Configuration() {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
	
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (GPIOA, &GPIO_InitStruct);
}

void TM1640_EndTransfer() {
    GPIO_ResetBits (GPIOA, DIN);
    GPIO_SetBits (GPIOA, SCLK);
    delay_ms (1);
    GPIO_SetBits (GPIOA, DIN);
}

void TM1640_StartTransfer() {
    TM1640_EndTransfer();
    GPIO_ResetBits (GPIOA, DIN);
    delay_ms (1);
    GPIO_ResetBits (GPIOA, SCLK);
}

void TM1640_Transfer (uint8_t data) {
	int i;
	
	for (i = 0; i < 8; i++) {
		GPIO_WriteBit (GPIOA, SCLK, (BitAction) 0);
		delay_ms (1);
        GPIO_WriteBit (GPIOA, DIN, (BitAction) ((data >> i) & 1));
        GPIO_WriteBit (GPIOA, SCLK, (BitAction) 1);
        delay_ms (1);
        GPIO_WriteBit (GPIOA, SCLK, (BitAction) 0);
    }
}

void TM1640_SendCmd (uint8_t data) {
    TM1640_StartTransfer();
    TM1640_Transfer (data);
    TM1640_EndTransfer();
}


void TM1640_Display (u8 add, u8 ptr) {
	TM1640_StartTransfer();
	TM1640_Transfer(0xC0 + add);
	TM1640_Transfer(buff[ptr]);
	TM1640_EndTransfer();
}

void TM1640_Init () {
    int i;
	TM1640_SendCmd(0x44);
    for (i = 0; i < 8; i++) {
        TM1640_Display (i, 20);
    }
	TM1640_SendCmd(0x8f);
}

int main (void) {   //主程序
    RCC_Configuration();
    GPIO_Configuration();
    TM1640_Init ();

    TM1640_Display (0,6);
	TM1640_Display (1,6);
	TM1640_Display (2,6);
	TM1640_Display (3,6);
	TM1640_Display (4,6);
	TM1640_Display (5,6);
	TM1640_Display (6,6);
	TM1640_Display (7,6);
    while (1);
}
