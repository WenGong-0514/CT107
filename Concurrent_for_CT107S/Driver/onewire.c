/********************************** (C) COPYRIGHT *******************************
 * File Name          : onewire.c
 * Author             : 0x49181f36
 * Version            : V0.0.2
 * Date               : 2024/03/26
 * Description        : ds18b20 driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

/*	# 	�����ߴ���Ƭ��˵��
	1. 	���ļ������ṩ���������빩����ѡ����ɳ�����Ʋο���
	2. 	����ѡ�ֿ������б�д��ش�����Ըô���Ϊ������������ѡ��Ƭ�����͡������ٶȺ�����
		�жԵ�Ƭ��ʱ��Ƶ�ʵ�Ҫ�󣬽��д�����Ժ��޸ġ�
*/


#include "onewire.h"

//
void Delay_OneWire(unsigned int t)  
{
    unsigned char i;
    while(t--)
    {
        for(i=0;i<12;i++);
    }
}

//
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
        DQ = dat&0x01;
        Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}


//
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

#ifdef Enbale_DS18B20_Precision_adjustment
void set_ds18b20()
{
    init_ds18b20();
    Write_DS18B20(0xCC);
    Write_DS18B20(0x4e);
    Write_DS18B20(0x7f);
    Write_DS18B20(0x00);
    Write_DS18B20(0x1f);
    init_ds18b20();
}
#endif


unsigned int rd_temperature() 
{ 
    unsigned char low, high;
    init_ds18b20();
    Write_DS18B20(0xCC); 
    Write_DS18B20(0x44); 
    Delay_OneWire(2);
    init_ds18b20(); 
    Write_DS18B20(0xCC); 
    Write_DS18B20(0xBE);
    low = Read_DS18B20(); 
    high = Read_DS18B20();
    return (high<<8)+low;
} 

