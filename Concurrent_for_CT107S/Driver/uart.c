/********************************** (C) COPYRIGHT *******************************
 * File Name          : uart.c
 * Author             : 0x49181f36
 * Version            : V0.0.2
 * Date               : 2024/03/26
 * Description        : uart driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#include "uart.h"

xdata char TX1_Buffer[TX1_Lenth] = 0;	//发送缓冲
xdata char RX1_Buffer[RX1_Lenth] = 0;	//接收缓冲
unsigned char  B_TX1_busy;		//忙标志  0空闲 1开始发送 2已经开始发送了但是还没发送完
unsigned char  B_RX1_flag = 0;//!255表示正在接收数据 255接收完成请处理

void uart_init()
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x8F;		//设置定时初始值
	T2H = 0xFD;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
  ES = 1;
}

void uart_0(void) interrupt 4
{
	if(RI)
  {
		RI = 0;
    RX1_Buffer[B_RX1_flag]=SBUF;
    if(RX1_Buffer[B_RX1_flag]==0x0A&&RX1_Buffer[B_RX1_flag-1]==0x0D)
    {
      B_RX1_flag=255;
    }
    else B_RX1_flag++;
  }
  if(TI)
  {
    TI = 0;
  }
}
