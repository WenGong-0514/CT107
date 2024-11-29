/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : 0x49181f36
 * Version            : V1.0.2
 * Date               : 2024/11/29
 * Description        : Main program body.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#include "STC15F2K60S2.h"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"
#include "ultrasonic.h"
#include "uart.h"
#include "stdio.h"

#define STC_DISPLAY //STC数码管调试接口开关

unsigned char Key_Read(void);

void Timer0Init(void);//NE555计数
void Timer1Init(void);//超声波计时

#define idle 0
#define read 1
#define write 2

//一下为模块时间控制 可以更改来调整模块调用速度
#define RTC_Task_time 299
#define PCF8591_Task_time 49
#define DS18B20_Task_time 249
#define NE555_Task_time 333
#define ULTRASONIC_Task_time 199
#define UART_Task_time 3

//用户自定义定时器 单位ms 不严格时间 ±10ms
#define User_TIM1 1000
#define User_TIM2 100

//PCA定时器周期中断 单位 us CCAP1是系统节拍定时器 CCAP2是显示定时器 请确保CCAP2数值小于CCAP1
#define TIME_CCAP1 1000
#define TIME_CCAP2 666
unsigned int TIME_1 = TIME_CCAP1;
unsigned int TIME_2 = TIME_CCAP2;
//字库
unsigned char code Code[38]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0X88,0X83,0XC6,0XA1,0X86,0X8E,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x3f,0x7F,0xBF,0xFF,0xAB,0xC7,0x8C};
//                              0   1    2    3    4    5   6     7     8   9   A     B   C     D   E   F      0.   1.  2.  3.     4.   5.  6.    7.   8.  9.   A.    B.    C. D.   E.     F.   .   -   空   n     L    P

//硬件抽象
unsigned char led;                              //LED显示
unsigned char seg[8]={34,34,34,34,34,34,34,34}; //数码管显示
unsigned char b_and_r;                          //蜂鸣器和继电器对应74HC573写入数据
unsigned char rtc[3] = {23, 59, 50};            //DS1302时间数据
unsigned char pcf8591[5]={0,0,0,0,128};         //PCF8591ADC内容以及DAC内容 [0]_右侧ADC接口 [1]_光敏电阻 [2]_LM324运放 [3]_电位器 [4]_DAC输出数据
unsigned char at24c02_read[2]={0,0};            //读取AT24C02数据 [0]_地址 [1]_读出的内容放置在这里
unsigned char at24c02_write[2]={0,0};           //写入AT24C02数据 [0]_地址 [1]_要写入的数值
unsigned char at24C02_control = idle;           //AT24C02读取写入控制器 将其赋值为read 则将at24c02_read[0]地址里的内容写入到at24c02_read[1]变量中(操作一次) 将其赋值为write 则将at24c02_write[1]的内容写入到at24c02_write[0]的地址内(操作一次) 读取写入操作完成自动转换为idle
unsigned long int ds18b20=220000;               //DS18B20温度
unsigned int ne555;                             //NE555频率
unsigned int wave;                              //超声波距离
unsigned char key_val,key_down,key_up;          //按键 val为实时按键状态 down监控按键按下瞬间 up监控按键抬起瞬间


unsigned char key_old;//按键旧值 无使用价值

//用户层界面切换控制逻辑
unsigned char mode=0;
unsigned char one_tag=255;

//模块控制使用 一般不需要更改
unsigned char SEG_Count;          //数码管扫描用
unsigned char KEY_Task;           //按键消抖用
unsigned int  RTC_Task;           //DS1302用
unsigned char PCF8591_Task;       //PCF8591用
unsigned char DS18B20_Task;       //DS18B20用
unsigned int  NE555_Task;         //NE555用
unsigned char ULTRASONIC_Task;    //超声波用
unsigned char UART_Task;          //串口用

//软件定时器在这里定义时间控制器 注意数据类型能够包括您定的时间
unsigned int USER_TIM_1;
unsigned char USER_TIM_2;


int main()
{
    P0 = 0xff;
    P2 = P2 & 0x1f | 0x80;
    P2 &= 0x1f;
    
    P0 = 0x00;
    P2 = P2 & 0x1f | 0xa0;
    P2 &= 0x1f;

    //set_ds18b20();
    Set_RTC(rtc);
    EA = 1;
    Timer1Init();
    Timer0Init();
    uart_init();
    at24C02_control = read;//开启一次AT24C02读
    while(1)
    {
        //界面类型数码管逻辑
        switch(mode)
        {
            case 0://界面0
            {
                if(one_tag!=0){one_tag=0;}
            }
            break;
            case 1://界面1
            {
                if(one_tag!=1){one_tag=1;}
            }
            break;
             case 2://界面2
            {
                if(one_tag!=2){one_tag=2;}
            }
            break;
            case 3://界面3
            {
                if(one_tag!=3){one_tag=3;}
            }
            break;
        }
        //其他逻辑

        //按键逻辑
        if(key_down)//按键按下
        {
          key_down =0;//防止连续触发
        }
        
        //UTIM1//软件定时器的使用就像下面这个样子 如果大于计数值 就让他减去计数值 这样做能够消除累计误差
        if(USER_TIM_1 > User_TIM1)
        {
            USER_TIM_1 -= User_TIM1;
        }
        if(USER_TIM_2 >User_TIM2)
        {
            USER_TIM_2 -=User_TIM2;
        }
        //关于软件定时器的设置 请查找"//软件定时器"即可到达位置 按照提示即可添加
        //关于串口接收的设置 请查找"//串口接收" 即可到达位置 按照以往格式即可添加
        /****************************************************************APP_Code**************************************************************************************/
        //以下为具体模块原代码 如果您只是使用该程序 那么您无需了解其是如何运行的
        //key
        if(KEY_Task == 10)
        {
            KEY_Task = 0;
            key_val=Key_Read();
            key_down = key_val & (key_old ^ key_val);
            key_up = ~key_val & (key_old ^ key_val);
            key_old = key_val;
        }
        //ds1302
        if(RTC_Task == RTC_Task_time)
        {
            RTC_Task = 0;
            Read_RTC(rtc);
        }
        //pcf8591
        if(PCF8591_Task == PCF8591_Task_time)
        {
            PCF8591_Task = 0;
            PCF8591_Adc(pcf8591);
            PCF8591_Dac(pcf8591[4]);
        }
        //AT24C02
        if(at24C02_control)
        {
            if (at24C02_control == read)
            EEPROM_Read(&at24c02_read[1],at24c02_read[0],1);
            else if (at24C02_control == write)
            EEPROM_Write(&at24c02_write[1],at24c02_write[0],1);
            at24C02_control = idle;
        }
        
        //ds18b20
        if(DS18B20_Task == DS18B20_Task_time)
        {
            DS18B20_Task = 0;
            ds18b20 = rd_temperature();
            ds18b20 =(ds18b20>>4)*10000+(ds18b20&0x0f)*625;
        }
        //NE555
        if(NE555_Task == NE555_Task_time)
        {
            NE555_Task = 0;
            TR0 = 0;
            ne555 = TH0*768+TL0*3;
            TH0 = 0;
            TL0 = 0;
            TR0 = 1;
        }
        
        //ultrasonic
        if(ULTRASONIC_Task == ULTRASONIC_Task_time)
        {
            ULTRASONIC_Task = 0;
            Wave_Recv();
        }
        //uart Send control
        if(UART_Task == UART_Task_time && B_TX1_busy != 255)
        {
          UART_Task = 0;
          if(TX1_Buffer[B_TX1_busy]!=0x00 || B_TX1_busy < 16)//Non-empty
          {
            SBUF=TX1_Buffer[B_TX1_busy];//Send
            B_TX1_busy++;
          }
          else//Send over
          {
            for(;B_TX1_busy>0;B_TX1_busy--)TX1_Buffer[B_TX1_busy]=0;//clean buf
              SBUF= 0x00;
            B_TX1_busy=255;
          }
        }
        
        //uart reception control//串口接收
        if(B_RX1_flag==255)
        {
            if(RX1_Buffer[0]=='O'&&RX1_Buffer[1]=='P'&&RX1_Buffer[2]=='E'&&RX1_Buffer[3]=='N'&&RX1_Buffer[4]==':'&&RX1_Buffer[5]=='R')b_and_r|=0x10;
            B_RX1_flag=0;
        }
    }
}

void CCP_IRQHandler(void) interrupt 7
{
    if(CCF0)//超声波用
    {
        CCF0=0;
        if(RX_Wait_FLAG)
        {
            TR1 = 0;
            if(TF1)TF1 = 0;
            else
            {
                 wave = (int)((TH1<<8)+TL1)*0.017;
                 RX_Wait_FLAG=0;
            }
        }
    }
    if(CCF1)//系统节拍
    {
        CCF1 = 0;
        CCAP1L = TIME_1;
        CCAP1H = TIME_1>>8;
        TIME_1 += TIME_CCAP1;
        if(KEY_Task < 10) KEY_Task++;
        if(RTC_Task < RTC_Task_time) RTC_Task++;
        if(PCF8591_Task < PCF8591_Task_time) PCF8591_Task++;
        if(DS18B20_Task < DS18B20_Task_time) DS18B20_Task++;
        if(NE555_Task < NE555_Task_time) NE555_Task++;
        if(ULTRASONIC_Task < ULTRASONIC_Task_time) ULTRASONIC_Task++;
        if(UART_Task < UART_Task_time) UART_Task++;
        //软件定时器加在这里 为什么不停止++呢-是为了防止累计误差 所以在数值达到后 请将时间减去您定的时间 这样 累计误差即可消除
        USER_TIM_1++;
        USER_TIM_2++;
    }
    if(CCF2)//关于显示(高实时低阻塞)
    {
        CCF2 = 0;
        CCAP2L = TIME_2;
        CCAP2H = TIME_2>>8;
        TIME_2 += TIME_CCAP2;
        
        //led
        P0=~led;
        P2&=0x1f;
        P2|=0x80;
        P2&=0x1f;
        
        //seg
        P0=0xff;
        P2&=0x1f;
        P2|=0xE0;
        P2&=0x1f;
        P0=1<<SEG_Count%8;
        P2&=0x1f;
        P2|=0xC0;
        P2&=0x1f;
        P0=Code[seg[SEG_Count%8]];
        P2&=0x1f;
        P2|=0xE0;
        P2&=0x1f;
        SEG_Count++;
        
        //bar
        P0 = b_and_r;
        P2&=0x1f;
        P2|=0xA0;
        P2 &= 0x1f;
    }
}

unsigned char Key_Read(void)
{
    P44=0;P42=1;P35=1;//P34=1;
    P33=1;P32=1;//P30=1;P31=1;
    if(!P33)return 4;
    else if(!P32)return 5;
//    else if(!P31)return 6;
//    else if(!P30)return 7;
    P44=1;P42=0;
    if(!P33)return 8;
    else if(!P32)return 9;
//    else if(!P31)return 10;
//    else if(!P30)return 11;
    P42=1;P35=0;
    if(!P33)return 12;
    else if(!P32)return 13;
//    else if(!P31)return 14;
//    else if(!P30)return 15;
//    P35=1;P34=0;
//    if(!P33)return 16;
//    else if(!P32)return 17;
//    else if(!P31)return 18;
//    else if(!P30)return 19;
    return 0;
}

void Timer0Init(void)
{
    AUXR |= 0x80;
	TMOD &= 0xF0;		//定时器时钟1T模式
	TMOD |= 0x04;		//设置定时器模式
	TL0 = 0;		//设置定时初始值
	TH0 = 0;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}
void PCA_TIME_init()
{
    P_SW1 &= 0xCF;//开启PCA
    CCAPM0 = 0x11;
    PPCA=1;//提高优先级
    CMOD = 0x00;
    CCON = 0x00;
    CH = 0;
    CL = 0;
    CCAP1L = TIME_1;
    CCAP1H = TIME_1>>8;
    CCAPM1 = 0x49;
    TIME_1 +=TIME_CCAP1;
    CCAP2L = TIME_1;
    CCAP2H = TIME_2>>8;
    CCAPM2 = 0x49;
    TIME_2 +=TIME_CCAP2;
    CR = 1;
}

void Timer1Init(void)		//
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0x00;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 0;		//定时器1不开始计时
    PCA_TIME_init();
}
