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

extern xdata char TX1_Buffer[TX1_Lenth];	//���ͻ���
extern xdata char RX1_Buffer[RX1_Lenth];	//���ջ���
extern unsigned char  B_TX1_busy;		//æ��־  0���� 1��ʼ���� 2�Ѿ���ʼ�����˵��ǻ�û������
extern unsigned char  B_RX1_flag;


void uart_init();



#endif