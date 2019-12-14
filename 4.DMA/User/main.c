/*****************
* DMA
* DT9025A
* 19/12/14
*****************/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h" //STM32ͷ�ļ�
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
    char buffer[200 + 1]; // ���ݳ���
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
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init (USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig (USART1, USART_IT_RXNE, ENABLE); //����ENABLE/�ر�DISABLE�ж�
    USART_Cmd (USART1, ENABLE);                   //ʹ�ܴ���
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

int main (void) {   //������
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
