# 一键切换闪灯模式

实现方法：GPIO + 中断

涉及到了 **NVIC** 和 **EXTI** **（划重点）**

1. 配置IO口(略)
2. 调用`RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE)`开启AFIO接口复用
3. 配置EXTI及NVIC
   1. EXTI
      1. 通过`GPIO_EXTILineConfig`开启对应中断线
      2. 通过`EXTI_InitTypeDef`以及`EXTI_Init`对EXTI进行配置
   2. 
      1. 通过`NVIC_PriorityGroupConfig`配置NVIC优先级分组
      2. 通过`NVIC_InitTypeDef`以及`NVIC_Init`对EXTI进行配置
4. 实现中断服务程序
   1. 注：在中断服务程序结束前要调用`EXTI_ClearITPendingBit`清除挂起位

### NVIC

---

NVIC(Nested Vectored Interrupt Controller)，即嵌套中断向量控制器，是Cortex-M3系列控制器内部独有集成单元，与CPU结合紧密，降低中断延迟时间并且能更加高效处理后续中断。它提供以下特征： 支持嵌套和向量中断、自动保存和恢复处理器状态、动态改变优先级、简化的和确定的中断时间。

当多个中断同时发生时，NVIC中的**抢占优先级**（也称主优先级/先占优先级）和**响应优先级**（也称子优先级/从优先级）就充当了一个判断、决定先执行哪个中断的角色。抢占优先级，顾名思义，就是能抢走其他中断的执行权，实现中断嵌套。响应优先级则在被抢后来决定一下执行顺序，不能抢占其他的位置，即不能嵌套。抢占优先级 > 响应优先级。

不同的中断优先级组，其所能设置的抢占、响应优先级的数量也不同。对此有兴趣的可以自行查找资料了解。下表列出了STM32F103系列所支持的优先级组。


|  NVIC_PriorityGroup  | NVIC_IRQChannel的先占优先级 | NVIC_IRQChannel的从优先级 |           描述            |
| :------------------: | :-------------------------: | :-----------------------: | :-----------------------: |
| NVIC_PriorityGroup_0 |              0              |           0-15            | 先占优先级0位 从优先级4位 |
| NVIC_PriorityGroup_1 |             0-1             |            0-7            | 先占优先级1位 从优先级3位 |
| NVIC_PriorityGroup_2 |             0-3             |            0-3            | 先占优先级2位 从优先级2位 |
| NVIC_PriorityGroup_3 |             0-7             |            0-1            | 先占优先级3位 从优先级1位 |
| NVIC_PriorityGroup_4 |            0-15             |             0             | 先占优先级4位 从优先级0位 |

注：

1. 选中NVIC_PriorityGroup_0，则参数NVIC_IRQChannelPreemptionPriority对中断通道的设置不产生影响。
2. 选中NVIC_PriorityGroup_4，则参数NVIC_IRQChannelSubPriority对中断通道的设置不产生影响。



在STM32中是通过改变CPU优先级来允许和禁止中断，进行开关总中断操作的。

（1）  下面两个函数等效关闭总中断

`void NVIC_SETPRIMASK(void);`
`void NVIC_SETFAULTMASK(void);`

（2）  下面两个函数等效开放总中断

`void NVIC_RESETPRIMASK(void);`
`void NVIC_RESETFAULTMASK(void);`

（3）  常用操作是先关后开中断

`NVIC_SETPRIMASK();`
`NVIC_RESETPRIMASK();`



关于NVIC的通俗易懂理解可以参考[这里](https://blog.csdn.net/qlexcel/article/details/78841296)，较专业参考[这里](https://www.cnblogs.com/121792730applllo/p/5155368.html)

NVIC有关库函数位于《固件库手册》的163页，NVIC的详细说明，请参考另一篇ST的文档和一篇ARM的文档：[《STM32F10xxx Cortex-M3编程手册》](http://www.st.com/stonline/products/literature/pm/15491.pdf)和[《Cortex™-M3技术参考手册》](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0337e/DDI0337E_cortex_m3_r1p1_trm.pdf)。

```c
//NVIC有关定义和操作函数
typedef struct
{
  uint8_t NVIC_IRQChannel;						//中断通道
  uint8_t NVIC_IRQChannelPreemptionPriority;	//抢占优先级
  uint8_t NVIC_IRQChannelSubPriority;			//响应优先级
  FunctionalState NVIC_IRQChannelCmd;			////使能/失能NVIC通道
} NVIC_InitTypeDef;

/****************************************************
				NVIC_IRQChannel取值
	WWDG_IRQChannel				窗口看门狗中断
	PVD_IRQChannelPVD			通过EXTI探测中断
	TAMPER_IRQChannel			篡改中断
	RTC_IRQChannelRTC			全局中断
	FlashItf_IRQChannelFLASH	全局中断
	RCC_IRQChannelRCC			全局中断
	EXTI0_IRQChannel			外部中断线0中断
	EXTI1_IRQChannel			外部中断线1中断
	EXTI2_IRQChannel			外部中断线2中断
	EXTI3_IRQChannel			外部中断线3中断
	EXTI4_IRQChannel			外部中断线4中断
	DMAChannel1_IRQChannel		DMA通道1中断
	DMAChannel2_IRQChannel		DMA通道2中断
	DMAChannel3_IRQChannel		DMA通道3中断
	DMAChannel4_IRQChannel		DMA通道4中断
	DMAChannel5_IRQChannel		DMA通道5中断
	DMAChannel6_IRQChannel		DMA通道6中断
	DMAChannel7_IRQChannel		DMA通道7中断
	ADC_IRQChannelADC			全局中断
	USB_HP_CANTX_IRQChannel		USB高优先级或者CAN发送中断
	USB_LP_CAN_RX0_IRQChannel	USB低优先级或者CAN接收0中断
	CAN_RX1_IRQChannel			CAN接收1中断
	CAN_SCE_IRQChannel			CAN SCE中断
	EXTI9_5_IRQChannel			外部中断线9-5中断
	TIM1_BRK_IRQChannel			TIM1暂停中断
	TIM1_UP_IRQChannel			TIM1刷新中断
	TIM1_TRG_COM_IRQChannel		TIM1触发和通讯中断
	TIM1_CC_IRQChannel			TIM1捕获比较中断
	TIM2_IRQChannel				TIM2全局中断
	TIM3_IRQChannel				TIM3全局中断
	TIM4_IRQChannel				TIM4全局中断
	I2C1_EV_IRQChannel			I2C1事件中断
	I2C1_ER_IRQChannel			I2C1错误中断
	I2C2_EV_IRQChannel			I2C2事件中断
	I2C2_ER_IRQChannel			I2C2错误中断
	SPI1_IRQChannel				SPI1全局中断
	SPI2_IRQChannel				SPI2全局中断
	USART1_IRQChannel			USART1全局中断
	USART2_IRQChannel			USART2全局中断
	USART3_IRQChannel			USART3全局中断
	EXTI15_10_IRQChannel		外部中断线15-10中断
	RTCAlarm_IRQChannel			RTC闹钟通过EXTI线中断
	USBWakeUp_IRQChannel		USB通过EXTI线从悬挂唤醒中断
****************************************************/

void NVIC_PriorityGroupConfig(u32 NVIC_PriorityGroup);	//设置优先级分组
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);		//初始化NVIC
void NVIC_SETPRIMASK(void); 							//使能PRIMASK优先级：提升执行优先级至0,相当于失能总中断
void NVIC_SETFAULTMASK(void);							//使能FAULTMASK优先级：提升执行优先级至-1,相当于失能总中断
void NVIC_RESETPRIMASK(void); 							//失能PRIMASK优先级,相当于使能总中断
void NVIC_RESETFAULTMASK(void);							//失能FAULTMASK优先级,相当于使能总中断
```




### EXTI

---

EXTI (External interrupt/event controller)，即外部中断/事件控制器，管理了控制器的20个中断/事件线。每个中断/事件线都对应有一个边沿检测器，可以实现输入信号的上升沿检测和下降沿的检测。EXTI 可以实现对每个中断/事件线进行单独配置，可以单独配置为中断或者事件，以及触发事件的属性。

EXTI使用时需要实现对应的中断服务程序，**并在服务程序中使用`EXTI_ClearITPendingBit`清除中断挂起位。**

EXTI有关库函数位于《固件库手册》的98页，EXTI有关信息位于《数据手册》的134页

```C
//EXTI有关定义和操作函数
typedef enum
{
  EXTI_Mode_Interrupt = 0x00,	//中断形式
  EXTI_Mode_Event = 0x04		//事件形式
}EXTIMode_TypeDef;

typedef enum
{
  EXTI_Trigger_Rising = 0x08,			//上升沿中断
  EXTI_Trigger_Falling = 0x0C,  		//下降沿中断
  EXTI_Trigger_Rising_Falling = 0x10	//跳变中断
}EXTITrigger_TypeDef;

typedef struct
{
  uint32_t EXTI_Line;					//EXTI中断线
  EXTIMode_TypeDef EXTI_Mode;			//EXTI中断模式
  EXTITrigger_TypeDef EXTI_Trigger;		//EXTI触发模式
  FunctionalState EXTI_LineCmd;			//使能/失能选中线路
}EXTI_InitTypeDef;

/****************************************************
	   EXTI_Line取值			 说明		  对应外部中断
		EXTI_Line0 			外部中断线0 		PX0
		EXTI_Line1 			外部中断线1 		PX1
		EXTI_Line2 			外部中断线2 		PX2
		EXTI_Line3 			外部中断线3 		PX3
		EXTI_Line4 			外部中断线4 		PX4
		EXTI_Line5 			外部中断线5 		PX5
		EXTI_Line6 			外部中断线6 		PX6
		EXTI_Line7			外部中断线7 		PX7
		EXTI_Line8			外部中断线8 		PX8
		EXTI_Line9			外部中断线9 		PX9
		EXTI_Line10			外部中断线10 	PX10
		EXTI_Line11 		外部中断线11 	PX11
		EXTI_Line12 		外部中断线12 	PX12
		EXTI_Line13 		外部中断线13 	PX13
		EXTI_Line14 		外部中断线14 	PX14
		EXTI_Line15 		外部中断线15 	PX15
		EXTI_Line16 		外部中断线16 	PVD输出
		EXTI_Line17 		外部中断线17 	RTC闹钟事件
		EXTI_Line18 		外部中断线18		USB唤醒事件
****************************************************/

void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);	//初始化EXTI
FlagStatus EXTI_GetFlagStatus(u32 EXTI_Line);		//取得指定EXTI线的标志位
ITStatus EXTI_GetITStatus(u32 EXTI_Line);			//检查指定的EXTI线路是否被触发
void EXTI_ClearFlag(u32 EXTI_Line);					//清除指定EXTI线的标志位
void EXTI_ClearITPendingBit(u32 EXTI_Line);			//清除指定EXTI线路挂起位

/**************************************************
			STM32F103的中断服务函数表速查
    WWDG_IRQHandler				Window Watchdog
	PVD_IRQHandler				PVD through EXTI Line detect
	TAMPER_IRQHandler			Tamper
	RTC_IRQHandler				RTC
	FLASH_IRQHandler			Flash
	RCC_IRQHandler				RCC
	EXTI0_IRQHandler			EXTI Line 0
	EXTI1_IRQHandler			EXTI Line 1
	EXTI2_IRQHandler			EXTI Line 2
	EXTI3_IRQHandler			EXTI Line 3
	EXTI4_IRQHandler			EXTI Line 4
	DMA1_Channel1_IRQHandler	DMA1 Channel 1
	DMA1_Channel2_IRQHandler	DMA1 Channel 2
	DMA1_Channel3_IRQHandler	DMA1 Channel 3
	DMA1_Channel4_IRQHandler	DMA1 Channel 4
	DMA1_Channel5_IRQHandler	DMA1 Channel 5
	DMA1_Channel6_IRQHandler	DMA1 Channel 6
	DMA1_Channel7_IRQHandler	DMA1 Channel 7
	ADC1_2_IRQHandler			ADC1 & ADC2
	USB_HP_CAN1_TX_IRQHandler	USB High Priority or CAN1 TX
	USB_LP_CAN1_RX0_IRQHandler	USB Low Priority or CAN1 RX0
    CAN1_RX1_IRQHandler			CAN1 RX1
	CAN1_SCE_IRQHandler			CAN1 SCE
	EXTI9_5_IRQHandler			EXTI Line 9..5
	TIM1_BRK_TIM9_IRQHandler	TIM1 Break and TIM9
	TIM1_UP_TIM10_IRQHandler	TIM1 Update and TIM10
	TIM1_TRG_COM_TIM11_IRQHandler	TIM1 Trigger and Commutation and TIM11
	TIM1_CC_IRQHandler			TIM1 Capture Compare
	TIM2_IRQHandler				TIM2
	TIM3_IRQHandler				TIM3
	TIM4_IRQHandler				TIM4
	I2C1_EV_IRQHandler			I2C1 Event
	I2C1_ER_IRQHandler			I2C1 Error
	I2C2_EV_IRQHandler			I2C2 Event
	I2C2_ER_IRQHandler			I2C2 Error
	SPI1_IRQHandler				SPI1
	SPI2_IRQHandler				SPI2
	USART1_IRQHandler			USART1
	USART2_IRQHandler			USART2
	USART3_IRQHandler			USART3
	EXTI15_10_IRQHandler		EXTI Line 15..10
	RTCAlarm_IRQHandler			RTC Alarm through EXTI Line
	USBWakeUp_IRQHandler		USB Wakeup from suspend
	TIM8_BRK_TIM12_IRQHandler	TIM8 Break and TIM12
	TIM8_UP_TIM13_IRQHandler	TIM8 Update and TIM13
	TIM8_TRG_COM_TIM14_IRQHandler	TIM8 Trigger and Commutation and TIM14
	TIM8_CC_IRQHandler			TIM8 Capture Compare
	ADC3_IRQHandler				ADC3
	FSMC_IRQHandler				FSMC
	SDIO_IRQHandler				SDIO
	TIM5_IRQHandler				TIM5
	SPI3_IRQHandler				SPI3
	UART4_IRQHandler			UART4
	UART5_IRQHandler			UART5
	TIM6_IRQHandler				TIM6
	TIM7_IRQHandler				TIM7
	DMA2_Channel1_IRQHandler	DMA2 Channel1
	DMA2_Channel2_IRQHandler	DMA2 Channel2
	DMA2_Channel3_IRQHandler	DMA2 Channel3
	DMA2_Channel4_5_IRQHandler	DMA2 Channel4 & Channel5
**************************************************/
```

