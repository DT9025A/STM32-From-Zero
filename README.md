# 菜鸡的STM32重入门记录

在填 [桌面气象站](https://github.com/DT9025A/DesktopWeatherStation) 这个坑的时候，菜鸡发现自己突击的STM32知识真的不扎实（ 

于是就决定

从零开始

学习STM32

（机械系的垃圾真的有学STM32的必要吗？

---

#### 说明

模板工程文件夹为所有上传工程的基础模板。

所有工程都需要被覆盖到这里才能被正确编译。

（其实大佬们自己配置也可以

因为买了 [洋桃](http://doyoung.net/YT/index.html) 的开发板，所以很多都是现成的轮子拿来用的。所以会在很多文件里见到洋桃的版权声明。

---

##### 19/12/13 基本GPIO操作

###### GPIO点灯

`GPIO_Write`

`GPIO_SetBits`

`GPIO_ResetBits`

`GPIO_WriteBit`



按键点灯

`GPIO_ReadInputDataBit`

`GPIO_ReadInputData`

`GPIO_ReadOutputDataBit`

`GPIO_ReadOutputData`



###### 19/12/14 来点中断

一键切换闪灯模式

`NVIC`

`EXTI`

`GPIO`

