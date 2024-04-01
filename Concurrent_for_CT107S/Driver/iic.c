/********************************** (C) COPYRIGHT *******************************
 * File Name          : iic.c
 * Author             : 0x49181f36
 * Version            : V0.0.2
 * Date               : 2024/03/29
 * Description        : iic driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

/*	#   I2C代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/

#include "iic.h"

#define DELAY_TIME	5

//
void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);
}


//
void IIC_Start(void)
{
    SDA_IIC = 1;
    SCL_IIC = 1;
    IIC_Delay(DELAY_TIME);
    SDA_IIC = 0;
    IIC_Delay(DELAY_TIME);
    SCL_IIC = 0;	
}

//
void IIC_Stop(void)
{
    SDA_IIC = 0;
    SCL_IIC = 1;
    IIC_Delay(DELAY_TIME);
    SDA_IIC = 1;
    IIC_Delay(DELAY_TIME);
}

//
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL_IIC  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA_IIC  = 1;
        else SDA_IIC  = 0;
        IIC_Delay(DELAY_TIME);
        SCL_IIC = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL_IIC  = 0;
}

//
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL_IIC = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA_IIC) da |= 1;
	SCL_IIC = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;
}

//
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL_IIC  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA_IIC;
    SCL_IIC = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//
void IIC_SendAck(bit ackbit)
{
    SCL_IIC = 0;
    SDA_IIC = ackbit;
    IIC_Delay(DELAY_TIME);
    SCL_IIC = 1;
    IIC_Delay(DELAY_TIME);
    SCL_IIC = 0; 
    SDA_IIC = 1;
    IIC_Delay(DELAY_TIME);
}

#ifdef PCF8591_ADC
unsigned char PCF8591_Adc(void) 
{ 
    unsigned char temp; 
    IIC_Start(); 
    IIC_SendByte(0x90); 
    IIC_WaitAck(); 
    IIC_SendByte(0x43);
    IIC_WaitAck(); 
    IIC_Start(); 
    IIC_SendByte(0x91); 
    IIC_WaitAck(); 
    temp = IIC_RecByte(); 
    IIC_SendAck(1); 
    IIC_Stop(); 
    return temp; 
} 
#endif 
#ifdef PCF8591_DAC 
void PCF8591_Dac(unsigned char dat) 
{ 
    IIC_Start(); 
    IIC_SendByte(0x90); 
    IIC_WaitAck(); 
    IIC_SendByte(0x43); 
    IIC_WaitAck(); 
    IIC_SendByte(dat); 
    IIC_WaitAck();
    IIC_Stop(); 
} 
#endif 

#ifdef EEPROM_AT24C02 
void EEPROM_Read(unsigned char* pucBuf, unsigned char addr, unsigned char num) 
{ 
    IIC_Start(); 
    IIC_SendByte(0xa0); 
    IIC_WaitAck(); 
    IIC_SendByte(addr); 
    IIC_WaitAck(); 
    IIC_Start(); 
    IIC_SendByte(0xa1); 
    IIC_WaitAck(); 
    while(num--) 
    { 
        *pucBuf++ = IIC_RecByte(); 
        if(num) IIC_SendAck(0); 
        else IIC_SendAck(1); 
    } 
    IIC_Stop(); 
} 
void EEPROM_Write(unsigned char* pucBuf, unsigned char addr, unsigned char num) 
{
    IIC_Start(); 
    IIC_SendByte(0xa0); 
    IIC_WaitAck(); 
    IIC_SendByte(addr); 
    IIC_WaitAck(); 
    while(num--) 
    { 
        IIC_SendByte(*pucBuf++); 
        IIC_WaitAck(); 
        IIC_Delay(200); 
    } 
    IIC_Stop();
} 
#endif
