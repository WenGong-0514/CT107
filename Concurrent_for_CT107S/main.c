/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : 0x49181f36
 * Version            : V1.0.1
 * Date               : 2024/03/30
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

/*
J2 2-4 1-3
J3 P34与SIGNAL连接
J5  矩阵或独立任意
J6 ON
J13 IO模式

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

#define STC_DISPLAY //STC数码管调试接口开关

unsigned char Key_Read(void);

void Timer0Init(void);//NE555计数
void Timer1Init(void);//超声波计时

//一下为模块时间控制 可以更改来调整模块调用速度
#define RTC_Task_time 299
#define PCF8591_Task_time 199
#define DS18B20_Task_time 249
#define NE555_Task_time 333
#define ULTRASONIC_Task_time 199
#define UART_Task_time 3

//用户自定义定时器 单位ms 不严格时间 ±10ms
#define User_TIM1 1000
#define User_TIM2 100
#define User_TIM3 66

//PCA定时器周期中断 单位 us CCAP1是系统节拍定时器 CCAP2是显示定时器 请确保CCAP2数值小于CCAP1
#define TIME_CCAP1 1000
#define TIME_CCAP2 666
unsigned int TIME_1 = TIME_CCAP1;
unsigned int TIME_2 = TIME_CCAP2;
//字库
unsigned char code Code[38]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0X88,0X83,0XC6,0XA1,0X86,0X8E,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x3f,0x7F,0xBF,0xFF,0xAB,0xC7,0x8C};
//                              0   1    2    3    4    5   6     7     8   9   A     B   C     D   E   F      0.   1.  2.  3.     4.   5.  6.    7.   8.  9.   A.    B.    C. D.   E.     F.   .   -   空   n     L    P

//HAL硬件抽象层
unsigned char led;
unsigned char seg[8]={34,34,34,34,34,34,34,34};
unsigned char key;
unsigned char b_and_r;
unsigned char Rtc[3] = {23, 59, 50};
unsigned char PCF8591[2]={128,128};
unsigned char at24c02[2]={0,0};
unsigned long int ds18b20=220000;
unsigned int NE555_Freq = 0;
//按键使用 您可以使用 key_val key_down key_up 他们的作用就是字面意思
unsigned char key_sanf_delay,key_old;
unsigned char key_val,key_down,key_up;

//用户层界面切换控制逻辑
unsigned char mode=0;
unsigned char one_tag=255;

//模块控制使用 一般不需要更改
unsigned char seg_count;//数码管扫描用
unsigned char KEY_Task;//按键消抖用
unsigned int  RTC_Task;//DS1302用
unsigned char PCF8591_Task;//PCF8591用
unsigned char DS18B20_Task;//DS18B20用
unsigned int  NE555_Task;//NE555用
unsigned char ULTRASONIC_Task;//超声波用
unsigned char UART_Task;//串口用

//软件定时器在这里定义时间控制器 注意数据类型能够包括您定的时间
unsigned int USER_TIM_1;
unsigned char USER_TIM_2;
unsigned char User_TIM_3;

bit AT24C02_Control = 0;//你可以给这个置1来读写EEPROM 读写地址需要去原代码处更改 默认0x00位置  查找"//AT24C02"来找到原代码


int main()
{
    P0 = 0xff;
    P2 = P2 & 0x1f | 0x80;
    P2 &= 0x1f;
    
    P0 = 0x00;
    P2 = P2 & 0x1f | 0xa0;
    P2 &= 0x1f;

    //set_ds18b20();
    Set_RTC(Rtc);
    EA = 1;
    Timer1Init();
    Timer0Init();
    uart_init();
    AT24C02_Control = 1;//开启一次AT24C02 既读又写
    while(1)
    {
        switch(mode)
        {
            case 0://界面0 AT24C02数据 0x00位置
            {
                 if(one_tag!=0){one_tag = 0;seg[0] = 14,seg[1] = 14; seg[2] = 37;seg[3] = 34;seg[4] = 34;}
                 seg[5] = at24c02[0]/100;
                 seg[6] = at24c02[0]/10%10;
                 seg[7] = at24c02[0]%10;
            }
            break;
            case 1://界面1 DS1302时钟
            {
                if(one_tag!=1){one_tag=1;seg[2] = 33;seg[5] = 33;b_and_r &= ~0x50;}
                seg[0] = Rtc[0]/10;
                seg[1] = Rtc[0]%10;
                seg[3] = Rtc[1]/10;
                seg[4] = Rtc[1]%10;
                seg[6] = Rtc[2]/10;
                seg[7] = Rtc[2]%10;
            }
            break;
             case 2://界面2 LED_TEST
            {
                if(one_tag!=2){one_tag=2;seg[0] = 36; seg[1] = 14; seg[2] = 13;seg[3] = 34;seg[4]=34;seg[5]= 34; seg[6]=34;seg[7]=34;}
                if(USER_TIM_2 > User_TIM2)
                {
                    USER_TIM_2 -= User_TIM2;
                    led ^= 0xFF;
                }
            }
            break;
            case 3://界面3 ADC
            {
                if(one_tag!=3){one_tag=3;seg[0] = 10; seg[1] = 13; seg[2] = 12;seg[3] = 34;led &= ~0xFF;}
                seg[5]=PCF8591[0]/100;
                seg[6]=PCF8591[0]/10%10;
                seg[7]=PCF8591[0]%10;
            }
            break;
            case 4://界面4 DS18B20数字温度计
            {
                if(one_tag!=4){one_tag=4;seg[0] = 12;seg[1] = 34;}
                seg[2]=ds18b20/100000%10;
                seg[3]=ds18b20/10000%10+16;
                seg[4]=ds18b20/1000%10;
                seg[5]=ds18b20/100%10;
                seg[6]=ds18b20/10%10;
                seg[7]=ds18b20%10;
            }
            break;
            case 5://界面5 超声波测距
            {
                if(one_tag!=5){one_tag=5;seg[0] = 12; seg[1] = 5; seg[2] = 11; seg[3] = 34;}
                seg[4]=wave_buf/1000;
                seg[5]=wave_buf/100%10;
                seg[6]=wave_buf/10%10;
                seg[7]=wave_buf%10;
            }
            break;
            case 6://界面6 NE555频率测量
            {
                if(one_tag!=6){one_tag=6;seg[0] =35; seg[1] = 14; seg[2] = 34;}
                seg[3]=NE555_Freq/10000;
                seg[4]=NE555_Freq/1000%10;
                seg[5]=NE555_Freq/100%10;
                seg[6]=NE555_Freq/10%10;
                seg[7]=NE555_Freq%10;
            }
            break;
            case 7://界面7 BEEP_AND_Relays_TEST
            {
                if(one_tag!=7){one_tag=7;seg[0] = 11; seg[1] = 14; seg[2] = 14;seg[3] = 37;seg[4]=34;seg[5]= 34; seg[6]=34;seg[7]=34;}
                if(USER_TIM_2 > User_TIM2)
                {
                    USER_TIM_2 -= User_TIM2;
                    b_and_r ^= 0x50;
                }
            }
            break;
        }

        if(key_down)//按键按下
        {
          if(key_down==4)mode++;//按下的是S4按键
          if(mode > 7)mode = 1;
          key_down =0;//防止连续触发
        }
        
        //UTIM1//软件定时器的使用就像下面这个样子 如果大于计数值 就让他减去计数值 这样做能够消除累计误差
        if(USER_TIM_1 > User_TIM1)
        {
            USER_TIM_1 -= User_TIM1;
            if(mode == 0)//开机显示1SEEPROM数值
            {
                mode = 1;
                at24c02[1] = at24c02[0]+1;//将读到的数据+1后写到准备写入的位置
                AT24C02_Control = 1;//再进行一次AT24C02操作
            }
            #ifndef STC_DISPLAY
            sprintf(TX1_Buffer,"超声波:%dcm ADC:%d 时间:%d-%d-%d 蜂鸣器:%d 继电器:%d 温度:%d\r\n",wave_buf,(unsigned int)PCF8591[0],(unsigned int)Rtc[0],(unsigned int)Rtc[1],(unsigned int)Rtc[2],(unsigned int)b_and_r>>6&0x01,(unsigned int)b_and_r>>4&0x01,(unsigned int)(ds18b20/10000));
            B_TX1_busy=0;//串口发送控制器 先"sprintf"将要发送的数据写入TX1_Buffer,然后给该位置0即可开始发送
            #endif
        }
        if(User_TIM_3 >User_TIM3)
        {
            User_TIM_3 -=User_TIM3;
            #ifdef STC_DISPLAY//STC_ISP数码管同步
            TX1_Buffer[0] = 0x37;
            TX1_Buffer[1] = 0x53;
            TX1_Buffer[2] = 0x45;
            TX1_Buffer[3] = 0x47;
            TX1_Buffer[4] = 0x43;
            TX1_Buffer[5] = 0x00;
            TX1_Buffer[6] = 0x00;
            TX1_Buffer[7] = 0x00;
            TX1_Buffer[8] = ~Code[seg[0]];
            TX1_Buffer[9] = ~Code[seg[1]];
            TX1_Buffer[10] = ~Code[seg[2]];
            TX1_Buffer[11] = ~Code[seg[3]];
            TX1_Buffer[12] = ~Code[seg[4]];
            TX1_Buffer[13] = ~Code[seg[5]];
            TX1_Buffer[14] = ~Code[seg[6]];
            TX1_Buffer[15] = ~Code[seg[7]];
            B_TX1_busy=0;
            #endif
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
            Read_RTC(Rtc);
        }
        //pcf8591
        if(PCF8591_Task == PCF8591_Task_time)
        {
            PCF8591_Task = 0;
            PCF8591[0]=PCF8591_Adc();
            PCF8591_Dac(PCF8591[1]);
        }
        //AT24C02
        if(AT24C02_Control)
        {
            AT24C02_Control = 0;
            EEPROM_Read(&at24c02[0],0x00,1);//Change yourself
            EEPROM_Write(&at24c02[1],0x00,1);//Change yourself
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
            NE555_Freq = TH0*768+TL0*3;
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
        #ifndef STC_DISPLAY
        //uart Send control
        if(UART_Task == UART_Task_time && B_TX1_busy != 255)
        {
          UART_Task = 0;
          if(TX1_Buffer[B_TX1_busy]!=0x00)//Non-empty
          {
            SBUF=TX1_Buffer[B_TX1_busy];//Send
            B_TX1_busy++;
          }
          else//Send over
          {
            for(;B_TX1_busy>0;B_TX1_busy--)TX1_Buffer[B_TX1_busy]=0;//clean buf
            B_TX1_busy=255;
          }
        }
        #else
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
        #endif
        
        //uart reception control//串口接收
        if(B_RX1_flag==255)
        {
            if(RX1_Buffer[0]=='O'&&RX1_Buffer[1]=='P'&&RX1_Buffer[2]=='E'&&RX1_Buffer[3]=='N'&&RX1_Buffer[4]==':'&&RX1_Buffer[5]=='R')b_and_r|=0x10;
            else if(RX1_Buffer[0]=='O'&&RX1_Buffer[1]=='P'&&RX1_Buffer[2]=='E'&&RX1_Buffer[3]=='N'&&RX1_Buffer[4]==':'&&RX1_Buffer[5]=='B')b_and_r|=0x40;
            else if(RX1_Buffer[0]=='C'&&RX1_Buffer[1]=='L'&&RX1_Buffer[2]=='O'&&RX1_Buffer[3]=='S'&&RX1_Buffer[4]=='E'&&RX1_Buffer[5]==':'&&RX1_Buffer[6]=='R')b_and_r&=~0x10;
            else if(RX1_Buffer[0]=='C'&&RX1_Buffer[1]=='L'&&RX1_Buffer[2]=='O'&&RX1_Buffer[3]=='S'&&RX1_Buffer[4]=='E'&&RX1_Buffer[5]==':'&&RX1_Buffer[6]=='B')b_and_r&=~0x40;
            else if(RX1_Buffer[0]=='L'&&RX1_Buffer[1]=='E'&&RX1_Buffer[2]=='D'&&RX1_Buffer[3]=='O'&&RX1_Buffer[4]=='P'&&RX1_Buffer[5]=='E'&&RX1_Buffer[6]=='N'&&RX1_Buffer[7]==':')led|=(0x01<<(RX1_Buffer[8]-0x30));
            else if(RX1_Buffer[0]=='L'&&RX1_Buffer[1]=='E'&&RX1_Buffer[2]=='D'&&RX1_Buffer[3]=='C'&&RX1_Buffer[4]=='L'&&RX1_Buffer[5]=='O'&&RX1_Buffer[6]=='S'&&RX1_Buffer[7]=='E'&&RX1_Buffer[8]==':')led&=~(0x01<<(RX1_Buffer[9]-0x30));
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
                 wave_buf = (int)((TH1<<8)+TL1)*0.017;
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
        User_TIM_3++;
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
        P0=1<<seg_count%8;
        P2&=0x1f;
        P2|=0xC0;
        P2&=0x1f;
        P0=Code[seg[seg_count%8]];
        P2&=0x1f;
        P2|=0xE0;
        P2&=0x1f;
        seg_count++;
        
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
