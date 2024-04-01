/********************************** (C) COPYRIGHT *******************************
 * File Name          : ultrasonic.c
 * Author             : 0x49181f36
 * Version            : V0.0.2
 * Date               : 2024/03/26
 * Description        : ultrasonic driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/


#include "ultrasonic.h"
#include "intrins.h"

sbit TX = P1^0;
sbit RX = P1^1;

unsigned int wave_buf;
bit RX_Wait_FLAG;



void DelaySonic(void)		//@12MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 29;
	while (--i);
}

void Wave_Recv(void)
{
    unsigned char ucNum = 20;

    if(RX_Wait_FLAG==1)wave_buf=9999;
    
    while(ucNum--)
    {
        TX ^= 0x01;
		DelaySonic();
    }
    
    TR1 = 0;
    TL1 = 0x00;		//设置定时初值
    TH1 = 0x00;		//设置定时初值
    TR1 = 1;
    
    RX_Wait_FLAG=1; 
}