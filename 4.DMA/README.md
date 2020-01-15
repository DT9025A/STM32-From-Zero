# DMA

DMA(Direct Memory Access)，即直接内存访问，数据手册对其定义为：提供在"外设和存储器之间"或者"存储器和存储器之间"的高速数据传输，无须CPU干预，数据可以通过DMA快速地移动，这就节省了CPU的资源来做其他操作。

DMA在很多场合有很大的用处，例如想要从USART读取一定数量的数据，如果不用DMA就需要对标志位进行轮询或者使用串口中断配合数组进行数据处理。这样，显然会占用CPU、打断主程序的运行，使产品的工作效率变低。而通过使用DMA则可以很好的解决这个问题：DMA独立于CPU之外，可以和CPU互不干扰地进行数据搬运的工作，而CPU要做的，仅仅是通过指令初始化并打开DMA。

DMA的配置很有讲究，要配置许多项目。先贴一个`DMA_InitTypeDef`的定义：

```c
typedef struct {
    u32 DMA_PeripheralBaseAddr;
    u32 DMA_MemoryBaseAddr; 
    u32 DMA_DIR; 
    u32 DMA_BufferSize; 
    u32 DMA_PeripheralInc; 
    u32 DMA_MemoryInc; 
    u32 DMA_PeripheralDataSize; 
    u32 DMA_MemoryDataSize; 
    u32 DMA_Mode; 
    u32 DMA_Priority; 
    u32 DMA_M2M; 
} DMA_InitTypeDef;
```

其每个成员的具体解释如下：

|        成员名称        |                             解释                             |                             用处                             |
| :--------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
| DMA_PeripheralBaseAddr |                 该参数用以定义DMA外设基地址                  |                 要进行操作的外设数据存放地址                 |
|   DMA_MemoryBaseAddr   |                 该参数用以定义DMA内存基地址                  |      要进行操作的内存数据存放地址（即数据指针所指地址）      |
|        DMA_DIR         |       该参数用以定义外设是作为数据传输的目的地还是来源       | 解释说的挺清楚的。可以取两个值：DMA_DIR_PeripheralDST（外设为数据目的地）和DMA_DIR_PeripheralSRC（外设为数据来源） |
|     DMA_BufferSize     | 该参数用以定义指定DMA通道的DMA缓存的大小，单位为数据单位。根据传输方向，数据单位等于结构中参数DMA_PeripheralDataSize或者参数DMA_MemoryDataSize的值 |                      指定想要搞多少数据                      |
|   DMA_PeripheralInc    |             该参数用以设定外设地址寄存器递增与否             | 当DMA搬运完一个数据后，外设方地址是否+1，有两取值：DMA_PeripheralInc_Enable（允许地址+1）和DMA_PeripheralInc_Disable（不允许地址+1） |
|     DMA_MemoryInc      |             该参数用以设定内存地址寄存器递增与否             | 作用同上，对象为内存地址。（此处《参考手册》存在错误：应为DMA_MemoryInc_Enable和DMA_MemoryInc_Disable） |
| DMA_PeripheralDataSize |                  该参数用以设定外设数据宽度                  | 指定对外设进行数据搬运时数据宽度，可取DMA_PeripheralDataSize[_Byte (8位), _HalfWord (16位), _Word(32位)] |
|   DMA_MemoryDataSize   |    该参数用以设定内存数据宽度（此处《参考手册》存在错误）    | 同上，对象为内存，取DMA_MemoryDataSize[_Byte (8位), _HalfWord (16位), _Word(32位)] |
|        DMA_Mode        |   该参数用以设定DMA的工作模式（此处《参考手册》存在错误）    | 规定DMA工作于循环模式（DMA_Mode_Circular）或是正常模式（DMA_Mode_Normal）（当指定DMA通道数据传输配置为内存到内存时，不能使用循环缓存模式，见Section DMA_M2M） |
|      DMA_Priority      |     该参数用以设定DMA通道x的软件优先级（x为当前DMA通道）     | 此成员有四个取值：该通道优先级非常高（DMA_Priority_VeryHigh）、优先级高（DMA_Priority_High）、优先级中（DMA_Priority_Medium）、优先级低（DMA_Priority_Low） |
|        DMA_M2M         |            该参数用以使能DMA通道的内存到内存传输             | DMA是否工作于内存-内存模式下，可取DMA_M2M_Enable、DMA_M2M_Disable |

在使用DMA进行操作前，要先开启对应DMA控制器（DMA1、DMA2）的时钟，然后对该通道进行重设寄存器缺省值操作（DMA_DeInit），然后对DMA进行初始化。当要使用DMA时，先使能DMA通道，然后开启对应外设的对应DMA通道。之后就可以交给DMA这个苦力来干活力！以下为一个例子。

```c
//例子：让DMA给USART搬运数据，进行发送

//......

//对DMA进行初始化
void DMA_Configuration() {
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);//开启DMA1时钟
    DMA_DeInit (DMA1_Channel4);//重设DMA1_14通道为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40013804;//设置外设内存地址 0x40013804即为(uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buffer;//设置内存地址为buffer,buffer为u8型数组变量
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//将外设设置为数据目的地
    DMA_InitStructure.DMA_BufferSize = 201;//要搬运201个数据
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//设定外设地址不自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//设定内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//内存数据8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//常规模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//非内存到内存
    DMA_Init (DMA1_Channel4, &DMA_InitStructure);//以上面的参数初始化DMA
}

//....

int main(){
    //....
    DMA_Cmd (DMA1_Channel4, ENABLE);//开启DMA1_14通道
    USART_DMACmd (USART1, USART_DMAReq_Tx, ENABLE);//开启USART的DMA TX通道
    while (1) {
        if (DMA_GetFlagStatus (DMA1_FLAG_TC4) != RESET) {//等待数据传送完成
            //....
        }
    }
}

//....
```

DMA有关库函数及定义在《固件库手册》的P86，DMA有关信息在《参考手册》的P142。

```c
//DMA有关定义和函数
typedef struct {
    u32 DMA_PeripheralBaseAddr;
    u32 DMA_MemoryBaseAddr; 
    u32 DMA_DIR; 
    u32 DMA_BufferSize; 
    u32 DMA_PeripheralInc; 
    u32 DMA_MemoryInc; 
    u32 DMA_PeripheralDataSize; 
    u32 DMA_MemoryDataSize; 
    u32 DMA_Mode; 
    u32 DMA_Priority; 
    u32 DMA_M2M; 
} DMA_InitTypeDef;

/********************************************
  DMA_FLAG        描述
DMA_FLAG_GL1 通道1全局标志位
DMA_FLAG_TC1 通道1传输完成标志位
DMA_FLAG_HT1 通道1传输过半标志位
DMA_FLAG_TE1 通道1传输错误标志位
DMA_FLAG_GL2 通道2全局标志位
DMA_FLAG_TC2 通道2传输完成标志位
DMA_FLAG_HT2 通道2传输过半标志位
DMA_FLAG_TE2 通道2传输错误标志位
DMA_FLAG_GL3 通道3全局标志位
DMA_FLAG_TC3 通道3传输完成标志位
DMA_FLAG_HT3 通道3传输过半标志位
DMA_FLAG_TE3 通道3传输错误标志位
DMA_FLAG_GL4 通道4全局标志位
DMA_FLAG_TC4 通道4传输完成标志位
DMA_FLAG_HT4 通道4传输过半标志位
DMA_FLAG_TE4 通道4传输错误标志位
DMA_FLAG_GL5 通道5全局标志位
DMA_FLAG_TC5 通道5传输完成标志位
DMA_FLAG_HT5 通道5传输过半标志位
DMA_FLAG_TE5 通道5传输错误标志位
DMA_FLAG_GL6 通道6全局标志位
DMA_FLAG_TC6 通道6传输完成标志位
DMA_FLAG_HT6 通道6传输过半标志位
DMA_FLAG_TE6 通道6传输错误标志位
DMA_FLAG_GL7 通道7全局标志位
DMA_FLAG_TC7 通道7传输完成标志位
DMA_FLAG_HT7 通道7传输过半标志位
DMA_FLAG_TE7 通道7传输错误标志位
********************************************/
/********************************************
  DMA_IT        描述
DMA_IT_GL1 通道1全局中断
DMA_IT_TC1 通道1传输完成中断
DMA_IT_HT1 通道1传输过半中断
DMA_IT_TE1 通道1传输错误中断
DMA_IT_GL2 通道2全局中断
DMA_IT_TC2 通道2传输完成中断
DMA_IT_HT2 通道2传输过半中断
DMA_IT_TE2 通道2传输错误中断
DMA_IT_GL3 通道3全局中断
DMA_IT_TC3 通道3传输完成中断
DMA_IT_HT3 通道3传输过半中断
DMA_IT_TE3 通道3传输错误中断
DMA_IT_GL4 通道4全局中断
DMA_IT_TC4 通道4传输完成中断
DMA_IT_HT4 通道4传输过半中断
DMA_IT_TE4 通道4传输错误中断
DMA_IT_GL5 通道5全局中断
DMA_IT_TC5 通道5传输完成中断
DMA_IT_HT5 通道5传输过半中断
DMA_IT_TE5 通道5传输错误中断
DMA_IT_GL6 通道6全局中断
DMA_IT_TC6 通道6传输完成中断
DMA_IT_HT6 通道6传输过半中断
DMA_IT_TE6 通道6传输错误中断
DMA_IT_GL7 通道7全局中断
DMA_IT_TC7 通道7传输完成中断
DMA_IT_HT7 通道7传输过半中断
DMA_IT_TE7 通道7传输错误中断
********************************************/

void DMA_DeInit(DMA_Channel_TypeDef* DMA_Channelx);//将DMA的通道x寄存器重设为缺省值
void DMA_Init(DMA_Channel_TypeDef* DMA_Channelx, DMA_InitTypeDef* DMA_InitStruct);//根据DMA_InitStruct中指定的参数初始化DMA的通道x寄存器
void DMA_Cmd(DMA_Channel_TypeDef* DMA_Channelx, FunctionalState NewState);//使能或者失能指定的通道x
void DMA_ITConfig(DMA_Channel_TypeDef* DMA_Channelx, u32 DMA_IT, FunctionalState NewState);//使能或者失能指定的通道x中断
u16 DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMA_Channelx);//返回当前DMA通道x剩余的待传输数据数目
FlagStatus DMA_GetFlagStatus(u32 DMA_FLAG);//检查指定的DMA通道x标志位设置与否
void DMA_ClearFlag(u32 DMA_FLAG);//清除DMA通道x待处理标志位
ITStatus DMA_GetITStatus(u32 DMA_IT);//检查指定的DMA通道x中断发生与否
void DMA_ClearITPendingBit(u32 DMA_IT);//清除DMA通道x中断待处理标志位
```

[这里](https://zhuanlan.zhihu.com/p/50767564)是DMA在USART接收不定长数据中的一个应用，有兴趣可自行阅读。