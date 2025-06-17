/********************************** (C) COPYRIGHT *******************************
 * File Name          : uart.c
 * Author             : 0x49181f36
 * Version            : V1.0.3
 * Date               : 2025/06/16
 * Description        : uart driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#include "uart.h"

//9600bps
void uart_init()
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC7;		//设置定时初始值
	T2H = 0xFE;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时
  ES = 1;
}