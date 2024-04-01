/********************************** (C) COPYRIGHT *******************************
 * File Name          : uart.h
 * Author             : 0x49181f36
 * Version            : V0.0.2
 * Date               : 2023/05/24
 * Description        : uart head.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#ifndef __uart_H_
#define __uart_H_

#include "STC15F2K60S2.h"

#define	TX1_Lenth	70
#define	RX1_Lenth	20

extern xdata char TX1_Buffer[TX1_Lenth];	//发送缓冲
extern xdata char RX1_Buffer[RX1_Lenth];	//接收缓冲
extern unsigned char  B_TX1_busy;		//忙标志  0空闲 1开始发送 2已经开始发送了但是还没发送完
extern unsigned char  B_RX1_flag;


void uart_init();



#endif