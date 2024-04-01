/********************************** (C) COPYRIGHT *******************************
 * File Name          : iic.h
 * Author             : 0x49181f36
 * Version            : V0.0.1
 * Date               : 2023/05/24
 * Description        : iic head.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#ifndef _IIC_H
#define _IIC_H

#include "STC15F2K60S2.h"
#include "intrins.h"

#define PCF8591_ADC//∆Ù”√ADC
#define PCF8591_DAC//∆Ù”√DAC
#define EEPROM_AT24C02//∆Ù”√EEPROM


sbit SDA_IIC = P2^1;
sbit SCL_IIC = P2^0;

void IIC_Start(void);
void IIC_Stop(void);
bit IIC_WaitAck(void);
void IIC_SendAck(bit ackbit);
void IIC_SendByte(unsigned char byt);
unsigned char IIC_RecByte(void);

unsigned char PCF8591_Adc(void);
void PCF8591_Dac(unsigned char dat);

void EEPROM_Read(unsigned char* pucBuf,unsigned char addr,unsigned char num); 
void EEPROM_Write(unsigned char* pucBuf,unsigned char addr,unsigned char num);


#endif