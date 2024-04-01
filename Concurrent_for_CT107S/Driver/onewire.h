/********************************** (C) COPYRIGHT *******************************
 * File Name          : onewire.h
 * Author             : 0x49181f36
 * Version            : V0.0.2
 * Date               : 2024/03/26
 * Description        : onewire head.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include "STC15F2K60S2.h"
#include "intrins.h"

//#define Enbale_DS18B20_Precision_adjustment

sbit DQ = P1^4;


void set_ds18b20();
unsigned int rd_temperature();  

#endif
