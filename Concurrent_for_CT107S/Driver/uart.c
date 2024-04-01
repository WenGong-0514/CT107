/********************************** (C) COPYRIGHT *******************************
 * File Name          : uart.c
 * Author             : 0x49181f36
 * Version            : V0.0.2
 * Date               : 2024/03/26
 * Description        : uart driver.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#include "uart.h"

xdata char TX1_Buffer[TX1_Lenth] = 0;	//���ͻ���
xdata char RX1_Buffer[RX1_Lenth] = 0;	//���ջ���
unsigned char  B_TX1_busy;		//æ��־  0���� 1��ʼ���� 2�Ѿ���ʼ�����˵��ǻ�û������
unsigned char  B_RX1_flag = 0;//!255��ʾ���ڽ������� 255��������봦��

void uart_init()
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x01;		//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0x8F;		//���ö�ʱ��ʼֵ
	T2H = 0xFD;		//���ö�ʱ��ʼֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
  ES = 1;
}

void uart_0(void) interrupt 4
{
	if(RI)
  {
		RI = 0;
    RX1_Buffer[B_RX1_flag]=SBUF;
    if(RX1_Buffer[B_RX1_flag]==0x0A&&RX1_Buffer[B_RX1_flag-1]==0x0D)
    {
      B_RX1_flag=255;
    }
    else B_RX1_flag++;
  }
  if(TI)
  {
    TI = 0;
  }
}
