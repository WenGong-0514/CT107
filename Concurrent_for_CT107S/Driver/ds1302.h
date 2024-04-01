/********************************** (C) COPYRIGHT *******************************
 * File Name          : ds1302.h
 * Author             : 0x49181f36
 * Version            : V0.0.1
 * Date               : 2023/05/24
 * Description        : ds1302 head.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#ifndef __DS1302_H
#define __DS1302_H

#include <stc15f2k60s2.h>
#include <intrins.h>

sbit SCK = P1^7;		
sbit SDA = P2^3;		
sbit RST = P1^3; 

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte( unsigned char address );
void Set_RTC(unsigned char* pucRtc);
void Read_RTC(unsigned char* pucRtc);

#endif
