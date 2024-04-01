/********************************** (C) COPYRIGHT *******************************
 * File Name          : ds2302.c
 * Author             : 0x49181f36
 * Version            : V0.0.1
 * Date               : 2023/05/24
 * Description        : ds1302 driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

/*	# 	DS1302代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/								

//   
#include "ds1302.h"
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

void Set_RTC(unsigned char* pucRtc)
{
    unsigned char temp;
    
    Write_Ds1302_Byte(0x8e,0);
    temp=((pucRtc[0]/10)<<4)+pucRtc[0]%10;
    Write_Ds1302_Byte(0x84,temp);
    temp=((pucRtc[1]/10)<<4)+pucRtc[1]%10;
    Write_Ds1302_Byte(0x82,temp);
    temp=((pucRtc[2]/10)<<4)+pucRtc[2]%10;
    Write_Ds1302_Byte(0x80,temp);
    Write_Ds1302_Byte(0x8e,0x90);
}

void Read_RTC(unsigned char* pucRtc)
{
    unsigned char temp;
    
    temp=Read_Ds1302_Byte(0x85);
    pucRtc[0]=(temp>>4)*10+(temp&0x0f);
    temp=Read_Ds1302_Byte(0x83);
    pucRtc[1]=(temp>>4)*10+(temp&0x0f);
    temp=Read_Ds1302_Byte(0x81);
    pucRtc[2]=(temp>>4)*10+(temp&0x0f);
}