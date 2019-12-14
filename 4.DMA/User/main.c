/*****************
* DMA
* DT9025A
* 19/12/14
*****************/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h" //STM32头文件
#include "sys.h"

//DMA TEST
u8 buffer[200];

void GPIO_Configuration() {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    //GPIOA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);

    //GPIOB
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOB, &GPIO_InitStructure);
}

void USART1_printf (char *fmt, ...) {
    char buffer[200 + 1]; // 数据长度
    u8 i = 0;
    va_list arg_ptr;
    va_start (arg_ptr, fmt);
    vsnprintf (buffer, 200 + 1, fmt, arg_ptr);
    while ((i < 200) && (i < strlen (buffer))) {
        USART_SendData (USART1, (u8) buffer[i++]);
        while (USART_GetFlagStatus (USART1, USART_FLAG_TC) == RESET);
    }
    va_end (arg_ptr);
}

void USART_Configuration (u32 bound) {
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init (USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig (USART1, USART_IT_RXNE, ENABLE); //开启ENABLE/关闭DISABLE中断
    USART_Cmd (USART1, ENABLE);                   //使能串口
}

void DMA_Configuration() {
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit (DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = 0X40013804;//(uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 201;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel4, &DMA_InitStructure);
}

int main (void) {   //主程序
    u8 us = 0;
    RCC_Configuration();
    GPIO_Configuration();
    USART_Configuration (115200);
    DMA_Configuration();
    USART1_printf (" Hello world\n");
    for (us = 0; us < 200; us++) {
        buffer[us] = '0' + us % 10;
    }
    USART1_printf ("Init Finished.\n");

    GPIO_ResetBits (GPIOB, GPIO_Pin_0);
    while (GPIO_ReadInputDataBit (GPIOA, GPIO_Pin_0) != 0);
    GPIO_SetBits (GPIOB, GPIO_Pin_0);
    USART1_printf ("DMA Transmission Start.\n");
    DMA_Cmd (DMA1_Channel4, ENABLE);
    USART_DMACmd (USART1, USART_DMAReq_Tx, ENABLE);

    while (1) {
        if (DMA_GetFlagStatus (DMA1_FLAG_TC4) != RESET) {
            USART1_printf ("\nDMA Transmission Finished\n");
            while (1);
        }
    }
}
