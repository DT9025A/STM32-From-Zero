# TM1640

1.TM1640是什么？

TM1640 是一种LED（发光二极管显示器）驱动控制与用电路，内部集成有MCU 数字接口、数据锁存器、LED 驱动等电路。本产品性能优良，质量可靠。主要应用于电子产品LED显示屏驱动。采用SOP28的封装形式。（棒读）



2.TM1640在哪？

![在这里(即答)](.\Picture\IMG_20200109_222320.jpg)

就这。



3.TM1640怎么驱动？

这个问题问得好。大家先来看这个接口说明：

![](.\Picture\Captured_1.png)

看一看，就是一个小小的串行协议：DIN与SCLK先后产生下降沿表示开始数据传输，数据在SCLK的上升沿被锁存，在SCLK的下降沿被释放。一次传输8个位（一个字节），由低位到高位。最后，SCLK和DIN先后产生上升沿，表示数据传输结束。

大家看！这里有一个老实的串行协议！一点都不花哨！

```c
#define SCLK GPIO_Pin_11
#define DIN GPIO_Pin_12

//起始TM1640数据传输
void TM1640_EndTransfer() {
    GPIO_ResetBits (GPIOA, DIN);
    GPIO_SetBits (GPIOA, SCLK);
    delay_ms (1);
    GPIO_SetBits (GPIOA, DIN);
}

//结束TM1640数据传输
void TM1640_StartTransfer() {
    TM1640_EndTransfer();
    GPIO_ResetBits (GPIOA, DIN);
    delay_ms (1);
    GPIO_ResetBits (GPIOA, SCLK);
}

//向TM1640传输数据
void TM1640_Transfer (uint8_t data) {
	int i;
	
	for (i = 0; i < 8; i++) {
		GPIO_WriteBit (GPIOA, SCLK, (BitAction) 0);//写时钟低
		delay_ms (1);
        GPIO_WriteBit (GPIOA, DIN, (BitAction) ((data >> i) & 1));//写数据
        GPIO_WriteBit (GPIOA, SCLK, (BitAction) 1);//写时钟高
        delay_ms (1);
        GPIO_WriteBit (GPIOA, SCLK, (BitAction) 0);//写时钟低
    }
}

```

TM1640有两种显示数据传输模式：连续地址和指定地址。对应时序如下图。

![连续地址](Picture\Captured_2.png)

![指定地址](Picture\Captured_3.png)

从上可以看出有些指令需要START+END。所以可以构建一个新函数，专门传输START+1BYTE+END指令。

```c
//向TM1640写指令
void TM1640_SendCmd (uint8_t data) {
    TM1640_StartTransfer();
    TM1640_Transfer (data);
    TM1640_EndTransfer();
}
```

本文只构建了针对指定地址的驱动程序。读者有兴趣可自行构建连续地址驱动。

```c
//显示段码数据
const u8 buff[21] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 
                     0x6f, 0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 
                     0xff, 0xef, 0x00};
//---------------      0    1      2    3      4    5      6    7      8    
//                     9    0.    1.   2.     3.   4.     5.   6.     7.   
//                     8.    9.   空白

//在指定位置显示指定数据
void TM1640_Display (u8 add, u8 ptr) {
	TM1640_StartTransfer();
	TM1640_Transfer(0xC0 + add);//地址开始于0xC0
	TM1640_Transfer(buff[ptr]);
	TM1640_EndTransfer();
}

//初始化
void TM1640_Init () {
    int i;
	TM1640_SendCmd(0x44);//指定地址模式
    for (i = 0; i < 8; i++) {
        TM1640_Display (i, 20);//清空显示
    }
	TM1640_SendCmd(0x8f);//亮度 最大
}
```

这次咕咕咕是有原因的！（超大声）

核心板的FT232菜掉了，买的STLINK好长时间才收到（哭唧唧