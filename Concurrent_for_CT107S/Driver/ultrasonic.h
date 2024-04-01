/********************************** (C) COPYRIGHT *******************************
 * File Name          : ultrasonic.h
 * Author             : 0x49181f36
 * Version            : V0.0.1
 * Date               : 2023/05/24
 * Description        : ultrasonic head.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#ifndef __Ultrasonic_H_
#define __Ultrasonic_H_

#include "STC15F2K60S2.h"

extern unsigned int wave_buf;
extern bit RX_Wait_FLAG;

void Timer1Init(void);
void Wave_Recv(void);

#endif