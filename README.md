# CT107
关于蓝桥杯CT107开发板的全模块同时使用硬件抽象模板

比赛时请你背默 请勿作弊插U盘 如果连学生比赛都作弊 就真的很无耻了

你可以免费使用但对此的更改与再分发必须署名原作者且你再更改后的程序必须同时开源并使用GPL3.0
![JU)M{QM4V2PX9JT11_PUJ)X](https://github.com/WenGong-0514/CT107/assets/130547806/e99849ed-11b0-41f3-9e70-95b3ffc617fb)

/*使用方法


//J2 2-4 1-3

//J3 P34与SIGNAL连接

//J5  矩阵或独立任意

//J6 ON

//J13 IO模式

使用内部IRC IRC频率12MHz
串口波特率4800

按键 S4 切换界面

界面0 EEPROM 0x00位置数值 显示1S时间

界面1 DS1302时钟 上电默认23-59-50

界面2 LED_Test 全部LED以0.1S为间隔闪烁

界面3 PCF8591 PCF8591读取RB2电压值

界面4 DS18B20 显示DS18B20温度最高精度数值

界面5 超声波测距 显示超声波测距数值

界面6 NE555 显示NE555输出频率数值

界面7 BEEP_AND_Relays_Test 继电器和蜂鸣器以0.1S为间隔开关

*/
具体使用方法可参照以下视频

https://www.bilibili.com/video/BV1jj421R7mu/

任何事宜可以通过邮箱stm32f103@qq.com联系我
