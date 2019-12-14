# GPIO点灯

基本GPIO操作顺序：

通过 `RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOx, ENABLE)` 开GPIO时钟

通过 `GPIO_InitTypeDef` 定义GPIO初始化信息，并通过 `GPIO_Init(GPIO_InitTypeDef*)` 初始化GPIO

然后就可以通过各种函数来操作GPIO啦！

```C
//有关定义和操作函数

typedef enum {
GPIO_Mode_AIN = 0x0, /* 模拟输入 */
GPIO_Mode_IN_FLOATING = 0x04, /* 输入浮空 */
GPIO_Mode_IPD = 0x28, /* 输入下拉 */
GPIO_Mode_IPU = 0x48, /* 输入上拉 */
GPIO_Mode_Out_OD = 0x14, /* 开漏输出 */
GPIO_Mode_Out_PP = 0x10, /* 推挽式输出 */
GPIO_Mode_AF_OD = 0x1C, /* 开漏复用 输出 */
GPIO_Mode_AF_PP = 0x18 /* 推挽式复用 输出 */
} GPIOMode_TypeDef;

typedef enum {
GPIO_Speed_10MHz = 1, //最高输出速率10MHz
GPIO_Speed_2MHz, //最高输出速率2MHz
GPIO_Speed_50MHz //最高输出速率50MHz
} GPIOSpeed_TypeDef;

typedef enum {
GPIO_Mode_AIN = 0x0, /* 模拟输入 */
GPIO_Mode_IN_FLOATING = 0x04, /* 输入浮空 */
GPIO_Mode_IPD = 0x28, /* 输入下拉 */
GPIO_Mode_IPU = 0x48, /* 输入上拉 */
GPIO_Mode_Out_OD = 0x14, /* 开漏输出 */
GPIO_Mode_Out_PP = 0x10, /* 推挽式输出 */
GPIO_Mode_AF_OD = 0x1C, /* 开漏复用 输出 */
GPIO_Mode_AF_PP = 0x18 /* 推挽式复用 输出 */
} GPIOMode_TypeDef;

typedef struct {
uint16_t GPIO_Pin; /* GPIO脚 */
GPIOSpeed_TypeDef GPIO_Speed; /* GPIO速度 */
GPIOMode_TypeDef GPIO_Mode; /* GPIO模式 */
} GPIO_InitTypeDef;

typedef enum {
	Bit_RESET = 0,
	Bit_SET
} BitAction;

/* 读管脚输入 */
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
/* 读管脚输出 */
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
/* 管脚输出 */
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);

```

