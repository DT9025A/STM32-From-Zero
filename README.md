# 菜鸡的STM32重入门记录

在填 [桌面气象站](https://github.com/DT9025A/DesktopWeatherStation) 这个坑的时候，菜鸡发现自己突击的STM32知识真的不扎实（ 

于是就决定

#### 从零开始

学习STM32

（机械系的垃圾真的有学STM32的必要吗？

---

### 目录

##### 19/12/13 基本GPIO操作

GPIO点灯 [1]

按键点灯 [2]

##### 19/12/14 来点中断 & 来点DMA

一键切换闪灯模式 [3]

单纯的DMA测试 [4]

##### 20/1/7 搞个数码管

TM1640驱动 [5]

---

### 说明

[模板工程] 文件夹为所有上传工程的基础模板，所有工程都需要被覆盖到这里才能被正确编译（当然其实大佬们自己配置也可以）

因为买了 [洋桃](http://doyoung.net/YT/index.html) 的开发板，所以很多都是现成的轮子拿来用的（所以会在很多文件里见到洋桃的版权声明）

缩略名称对应：

|                            文件名                            |                            文档名                            |     缩略名     |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :------------: |
|       参考资料\STM32F103固件函数库用户手册（中文）.pdf       | 32位基于ARM微控制器STM32F101xx与STM32F103xx 固件函数库 (UM0427 Oct. 2007 Rev 2) | 《固件库手册》 |
| 参考资料\STM32F10XXX参考手册（中文）-20150727-CD00171190_ZHV10.pdf | 参考手册</br> STM32F101xx, STM32F102xx、STM32F103xx、STM32F105xx 和STM32F107xx，ARM内核32位高性能微控制器（RM0008 Dec. 2009 Rev 10） |  《参考手册》  |
| 参考资料\STM32F103X8-B数据手册（中文）-20150727-CD00161566_ZHV10.pdf |         数据手册 </br> STM32F103x8 </br> STM32F103xB         |  《数据手册》  |
|           参考资料\TM1640_V1.2数据手册（中文）.pdf           |                 LED 驱动控制与用电路 TM1640                  |   《TM1640》   |

---

