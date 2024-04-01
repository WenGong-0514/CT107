/********************************** (C) COPYRIGHT *******************************
 * File Name          : ds2302.c
 * Author             : 0x49181f36
 * Version            : V0.0.1
 * Date               : 2023/05/24
 * Description        : ds1302 driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

/*	# 	DS1302����Ƭ��˵��
	1. 	���ļ������ṩ���������빩����ѡ����ɳ�����Ʋο���
	2. 	����ѡ�ֿ������б�д��ش�����Ըô���Ϊ������������ѡ��Ƭ�����͡������ٶȺ�����
		�жԵ�Ƭ��ʱ��Ƶ�ʵ�Ҫ�󣬽��д�����Ժ��޸ġ�
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