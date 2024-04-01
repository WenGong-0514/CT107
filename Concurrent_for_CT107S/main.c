/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : 0x49181f36
 * Version            : V1.0.1
 * Date               : 2024/03/30
 * Description        : Main program body.
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/

#include "STC15F2K60S2.h"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"
#include "ultrasonic.h"
#include "uart.h"
#include "stdio.h"

/*
J2 2-4 1-3
J3 P34��SIGNAL����
J5  ������������
J6 ON
J13 IOģʽ

ʹ���ڲ�IRC IRCƵ��12MHz
���ڲ�����4800

���� S4 �л�����

����0 EEPROM 0x00λ����ֵ ��ʾ1Sʱ��
����1 DS1302ʱ�� �ϵ�Ĭ��23-59-50
����2 LED_Test ȫ��LED��0.1SΪ�����˸
����3 PCF8591 PCF8591��ȡRB2��ѹֵ
����4 DS18B20 ��ʾDS18B20�¶���߾�����ֵ
����5 ��������� ��ʾ�����������ֵ
����6 NE555 ��ʾNE555���Ƶ����ֵ
����7 BEEP_AND_Relays_Test �̵����ͷ�������0.1SΪ�������
*/

#define STC_DISPLAY //STC����ܵ��Խӿڿ���

unsigned char Key_Read(void);

void Timer0Init(void);//NE555����
void Timer1Init(void);//��������ʱ

//һ��Ϊģ��ʱ����� ���Ը���������ģ������ٶ�
#define RTC_Task_time 299
#define PCF8591_Task_time 199
#define DS18B20_Task_time 249
#define NE555_Task_time 333
#define ULTRASONIC_Task_time 199
#define UART_Task_time 3

//�û��Զ��嶨ʱ�� ��λms ���ϸ�ʱ�� ��10ms
#define User_TIM1 1000
#define User_TIM2 100
#define User_TIM3 66

//PCA��ʱ�������ж� ��λ us CCAP1��ϵͳ���Ķ�ʱ�� CCAP2����ʾ��ʱ�� ��ȷ��CCAP2��ֵС��CCAP1
#define TIME_CCAP1 1000
#define TIME_CCAP2 666
unsigned int TIME_1 = TIME_CCAP1;
unsigned int TIME_2 = TIME_CCAP2;
//�ֿ�
unsigned char code Code[38]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90,0X88,0X83,0XC6,0XA1,0X86,0X8E,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x08,0x03,0x46,0x21,0x06,0x3f,0x7F,0xBF,0xFF,0xAB,0xC7,0x8C};
//                              0   1    2    3    4    5   6     7     8   9   A     B   C     D   E   F      0.   1.  2.  3.     4.   5.  6.    7.   8.  9.   A.    B.    C. D.   E.     F.   .   -   ��   n     L    P

//HALӲ�������
unsigned char led;
unsigned char seg[8]={34,34,34,34,34,34,34,34};
unsigned char key;
unsigned char b_and_r;
unsigned char Rtc[3] = {23, 59, 50};
unsigned char PCF8591[2]={128,128};
unsigned char at24c02[2]={0,0};
unsigned long int ds18b20=220000;
unsigned int NE555_Freq = 0;
//����ʹ�� ������ʹ�� key_val key_down key_up ���ǵ����þ���������˼
unsigned char key_sanf_delay,key_old;
unsigned char key_val,key_down,key_up;

//�û�������л������߼�
unsigned char mode=0;
unsigned char one_tag=255;

//ģ�����ʹ�� һ�㲻��Ҫ����
unsigned char seg_count;//�����ɨ����
unsigned char KEY_Task;//����������
unsigned int  RTC_Task;//DS1302��
unsigned char PCF8591_Task;//PCF8591��
unsigned char DS18B20_Task;//DS18B20��
unsigned int  NE555_Task;//NE555��
unsigned char ULTRASONIC_Task;//��������
unsigned char UART_Task;//������

//�����ʱ�������ﶨ��ʱ������� ע�����������ܹ�����������ʱ��
unsigned int USER_TIM_1;
unsigned char USER_TIM_2;
unsigned char User_TIM_3;

bit AT24C02_Control = 0;//����Ը������1����дEEPROM ��д��ַ��Ҫȥԭ���봦���� Ĭ��0x00λ��  ����"//AT24C02"���ҵ�ԭ����


int main()
{
    P0 = 0xff;
    P2 = P2 & 0x1f | 0x80;
    P2 &= 0x1f;
    
    P0 = 0x00;
    P2 = P2 & 0x1f | 0xa0;
    P2 &= 0x1f;

    //set_ds18b20();
    Set_RTC(Rtc);
    EA = 1;
    Timer1Init();
    Timer0Init();
    uart_init();
    AT24C02_Control = 1;//����һ��AT24C02 �ȶ���д
    while(1)
    {
        switch(mode)
        {
            case 0://����0 AT24C02���� 0x00λ��
            {
                 if(one_tag!=0){one_tag = 0;seg[0] = 14,seg[1] = 14; seg[2] = 37;seg[3] = 34;seg[4] = 34;}
                 seg[5] = at24c02[0]/100;
                 seg[6] = at24c02[0]/10%10;
                 seg[7] = at24c02[0]%10;
            }
            break;
            case 1://����1 DS1302ʱ��
            {
                if(one_tag!=1){one_tag=1;seg[2] = 33;seg[5] = 33;b_and_r &= ~0x50;}
                seg[0] = Rtc[0]/10;
                seg[1] = Rtc[0]%10;
                seg[3] = Rtc[1]/10;
                seg[4] = Rtc[1]%10;
                seg[6] = Rtc[2]/10;
                seg[7] = Rtc[2]%10;
            }
            break;
             case 2://����2 LED_TEST
            {
                if(one_tag!=2){one_tag=2;seg[0] = 36; seg[1] = 14; seg[2] = 13;seg[3] = 34;seg[4]=34;seg[5]= 34; seg[6]=34;seg[7]=34;}
                if(USER_TIM_2 > User_TIM2)
                {
                    USER_TIM_2 -= User_TIM2;
                    led ^= 0xFF;
                }
            }
            break;
            case 3://����3 ADC
            {
                if(one_tag!=3){one_tag=3;seg[0] = 10; seg[1] = 13; seg[2] = 12;seg[3] = 34;led &= ~0xFF;}
                seg[5]=PCF8591[0]/100;
                seg[6]=PCF8591[0]/10%10;
                seg[7]=PCF8591[0]%10;
            }
            break;
            case 4://����4 DS18B20�����¶ȼ�
            {
                if(one_tag!=4){one_tag=4;seg[0] = 12;seg[1] = 34;}
                seg[2]=ds18b20/100000%10;
                seg[3]=ds18b20/10000%10+16;
                seg[4]=ds18b20/1000%10;
                seg[5]=ds18b20/100%10;
                seg[6]=ds18b20/10%10;
                seg[7]=ds18b20%10;
            }
            break;
            case 5://����5 ���������
            {
                if(one_tag!=5){one_tag=5;seg[0] = 12; seg[1] = 5; seg[2] = 11; seg[3] = 34;}
                seg[4]=wave_buf/1000;
                seg[5]=wave_buf/100%10;
                seg[6]=wave_buf/10%10;
                seg[7]=wave_buf%10;
            }
            break;
            case 6://����6 NE555Ƶ�ʲ���
            {
                if(one_tag!=6){one_tag=6;seg[0] =35; seg[1] = 14; seg[2] = 34;}
                seg[3]=NE555_Freq/10000;
                seg[4]=NE555_Freq/1000%10;
                seg[5]=NE555_Freq/100%10;
                seg[6]=NE555_Freq/10%10;
                seg[7]=NE555_Freq%10;
            }
            break;
            case 7://����7 BEEP_AND_Relays_TEST
            {
                if(one_tag!=7){one_tag=7;seg[0] = 11; seg[1] = 14; seg[2] = 14;seg[3] = 37;seg[4]=34;seg[5]= 34; seg[6]=34;seg[7]=34;}
                if(USER_TIM_2 > User_TIM2)
                {
                    USER_TIM_2 -= User_TIM2;
                    b_and_r ^= 0x50;
                }
            }
            break;
        }

        if(key_down)//��������
        {
          if(key_down==4)mode++;//���µ���S4����
          if(mode > 7)mode = 1;
          key_down =0;//��ֹ��������
        }
        
        //UTIM1//�����ʱ����ʹ�þ�������������� ������ڼ���ֵ ��������ȥ����ֵ �������ܹ������ۼ����
        if(USER_TIM_1 > User_TIM1)
        {
            USER_TIM_1 -= User_TIM1;
            if(mode == 0)//������ʾ1SEEPROM��ֵ
            {
                mode = 1;
                at24c02[1] = at24c02[0]+1;//������������+1��д��׼��д���λ��
                AT24C02_Control = 1;//�ٽ���һ��AT24C02����
            }
            #ifndef STC_DISPLAY
            sprintf(TX1_Buffer,"������:%dcm ADC:%d ʱ��:%d-%d-%d ������:%d �̵���:%d �¶�:%d\r\n",wave_buf,(unsigned int)PCF8591[0],(unsigned int)Rtc[0],(unsigned int)Rtc[1],(unsigned int)Rtc[2],(unsigned int)b_and_r>>6&0x01,(unsigned int)b_and_r>>4&0x01,(unsigned int)(ds18b20/10000));
            B_TX1_busy=0;//���ڷ��Ϳ����� ��"sprintf"��Ҫ���͵�����д��TX1_Buffer,Ȼ�����λ��0���ɿ�ʼ����
            #endif
        }
        if(User_TIM_3 >User_TIM3)
        {
            User_TIM_3 -=User_TIM3;
            #ifdef STC_DISPLAY//STC_ISP�����ͬ��
            TX1_Buffer[0] = 0x37;
            TX1_Buffer[1] = 0x53;
            TX1_Buffer[2] = 0x45;
            TX1_Buffer[3] = 0x47;
            TX1_Buffer[4] = 0x43;
            TX1_Buffer[5] = 0x00;
            TX1_Buffer[6] = 0x00;
            TX1_Buffer[7] = 0x00;
            TX1_Buffer[8] = ~Code[seg[0]];
            TX1_Buffer[9] = ~Code[seg[1]];
            TX1_Buffer[10] = ~Code[seg[2]];
            TX1_Buffer[11] = ~Code[seg[3]];
            TX1_Buffer[12] = ~Code[seg[4]];
            TX1_Buffer[13] = ~Code[seg[5]];
            TX1_Buffer[14] = ~Code[seg[6]];
            TX1_Buffer[15] = ~Code[seg[7]];
            B_TX1_busy=0;
            #endif
        }
        //���������ʱ�������� �����"//�����ʱ��"���ɵ���λ�� ������ʾ�������
        //���ڴ��ڽ��յ����� �����"//���ڽ���" ���ɵ���λ�� ����������ʽ�������
        /****************************************************************APP_Code**************************************************************************************/
        //����Ϊ����ģ��ԭ���� �����ֻ��ʹ�øó��� ��ô�������˽�����������е�
        //key
        if(KEY_Task == 10)
        {
            KEY_Task = 0;
            key_val=Key_Read();
            key_down = key_val & (key_old ^ key_val);
            key_up = ~key_val & (key_old ^ key_val);
            key_old = key_val;
        }
        //ds1302
        if(RTC_Task == RTC_Task_time)
        {
            RTC_Task = 0;
            Read_RTC(Rtc);
        }
        //pcf8591
        if(PCF8591_Task == PCF8591_Task_time)
        {
            PCF8591_Task = 0;
            PCF8591[0]=PCF8591_Adc();
            PCF8591_Dac(PCF8591[1]);
        }
        //AT24C02
        if(AT24C02_Control)
        {
            AT24C02_Control = 0;
            EEPROM_Read(&at24c02[0],0x00,1);//Change yourself
            EEPROM_Write(&at24c02[1],0x00,1);//Change yourself
        }
        
        //ds18b20
        if(DS18B20_Task == DS18B20_Task_time)
        {
            DS18B20_Task = 0;
            ds18b20 = rd_temperature();
            ds18b20 =(ds18b20>>4)*10000+(ds18b20&0x0f)*625;
        }
        //NE555
        if(NE555_Task == NE555_Task_time)
        {
            NE555_Task = 0;
            TR0 = 0;
            NE555_Freq = TH0*768+TL0*3;
            TH0 = 0;
            TL0 = 0;
            TR0 = 1;
        }
        
        //ultrasonic
        if(ULTRASONIC_Task == ULTRASONIC_Task_time)
        {
            ULTRASONIC_Task = 0;
            Wave_Recv();
        }
        #ifndef STC_DISPLAY
        //uart Send control
        if(UART_Task == UART_Task_time && B_TX1_busy != 255)
        {
          UART_Task = 0;
          if(TX1_Buffer[B_TX1_busy]!=0x00)//Non-empty
          {
            SBUF=TX1_Buffer[B_TX1_busy];//Send
            B_TX1_busy++;
          }
          else//Send over
          {
            for(;B_TX1_busy>0;B_TX1_busy--)TX1_Buffer[B_TX1_busy]=0;//clean buf
            B_TX1_busy=255;
          }
        }
        #else
        //uart Send control
        if(UART_Task == UART_Task_time && B_TX1_busy != 255)
        {
          UART_Task = 0;
          if(TX1_Buffer[B_TX1_busy]!=0x00 || B_TX1_busy < 16)//Non-empty
          {
            SBUF=TX1_Buffer[B_TX1_busy];//Send
            B_TX1_busy++;
          }
          else//Send over
          {
            for(;B_TX1_busy>0;B_TX1_busy--)TX1_Buffer[B_TX1_busy]=0;//clean buf
              SBUF= 0x00;
            B_TX1_busy=255;
          }
        }
        #endif
        
        //uart reception control//���ڽ���
        if(B_RX1_flag==255)
        {
            if(RX1_Buffer[0]=='O'&&RX1_Buffer[1]=='P'&&RX1_Buffer[2]=='E'&&RX1_Buffer[3]=='N'&&RX1_Buffer[4]==':'&&RX1_Buffer[5]=='R')b_and_r|=0x10;
            else if(RX1_Buffer[0]=='O'&&RX1_Buffer[1]=='P'&&RX1_Buffer[2]=='E'&&RX1_Buffer[3]=='N'&&RX1_Buffer[4]==':'&&RX1_Buffer[5]=='B')b_and_r|=0x40;
            else if(RX1_Buffer[0]=='C'&&RX1_Buffer[1]=='L'&&RX1_Buffer[2]=='O'&&RX1_Buffer[3]=='S'&&RX1_Buffer[4]=='E'&&RX1_Buffer[5]==':'&&RX1_Buffer[6]=='R')b_and_r&=~0x10;
            else if(RX1_Buffer[0]=='C'&&RX1_Buffer[1]=='L'&&RX1_Buffer[2]=='O'&&RX1_Buffer[3]=='S'&&RX1_Buffer[4]=='E'&&RX1_Buffer[5]==':'&&RX1_Buffer[6]=='B')b_and_r&=~0x40;
            else if(RX1_Buffer[0]=='L'&&RX1_Buffer[1]=='E'&&RX1_Buffer[2]=='D'&&RX1_Buffer[3]=='O'&&RX1_Buffer[4]=='P'&&RX1_Buffer[5]=='E'&&RX1_Buffer[6]=='N'&&RX1_Buffer[7]==':')led|=(0x01<<(RX1_Buffer[8]-0x30));
            else if(RX1_Buffer[0]=='L'&&RX1_Buffer[1]=='E'&&RX1_Buffer[2]=='D'&&RX1_Buffer[3]=='C'&&RX1_Buffer[4]=='L'&&RX1_Buffer[5]=='O'&&RX1_Buffer[6]=='S'&&RX1_Buffer[7]=='E'&&RX1_Buffer[8]==':')led&=~(0x01<<(RX1_Buffer[9]-0x30));
            B_RX1_flag=0;
        }
    }
}

void CCP_IRQHandler(void) interrupt 7
{
    if(CCF0)//��������
    {
        CCF0=0;
        if(RX_Wait_FLAG)
        {
            TR1 = 0;
            if(TF1)TF1 = 0;
            else
            {
                 wave_buf = (int)((TH1<<8)+TL1)*0.017;
                 RX_Wait_FLAG=0;
            }
        }
    }
    if(CCF1)//ϵͳ����
    {
        CCF1 = 0;
        CCAP1L = TIME_1;
        CCAP1H = TIME_1>>8;
        TIME_1 += TIME_CCAP1;
        if(KEY_Task < 10) KEY_Task++;
        if(RTC_Task < RTC_Task_time) RTC_Task++;
        if(PCF8591_Task < PCF8591_Task_time) PCF8591_Task++;
        if(DS18B20_Task < DS18B20_Task_time) DS18B20_Task++;
        if(NE555_Task < NE555_Task_time) NE555_Task++;
        if(ULTRASONIC_Task < ULTRASONIC_Task_time) ULTRASONIC_Task++;
        if(UART_Task < UART_Task_time) UART_Task++;
        //�����ʱ���������� Ϊʲô��ֹͣ++��-��Ϊ�˷�ֹ�ۼ���� ��������ֵ�ﵽ�� �뽫ʱ���ȥ������ʱ�� ���� �ۼ���������
        USER_TIM_1++;
        USER_TIM_2++;
        User_TIM_3++;
    }
    if(CCF2)//������ʾ(��ʵʱ������)
    {
        CCF2 = 0;
        CCAP2L = TIME_2;
        CCAP2H = TIME_2>>8;
        TIME_2 += TIME_CCAP2;
        
        //led
        P0=~led;
        P2&=0x1f;
        P2|=0x80;
        P2&=0x1f;
        
        //seg
        P0=0xff;
        P2&=0x1f;
        P2|=0xE0;
        P2&=0x1f;
        P0=1<<seg_count%8;
        P2&=0x1f;
        P2|=0xC0;
        P2&=0x1f;
        P0=Code[seg[seg_count%8]];
        P2&=0x1f;
        P2|=0xE0;
        P2&=0x1f;
        seg_count++;
        
        //bar
        P0 = b_and_r;
        P2&=0x1f;
        P2|=0xA0;
        P2 &= 0x1f;
    }
}

unsigned char Key_Read(void)
{
    P44=0;P42=1;P35=1;//P34=1;
    P33=1;P32=1;//P30=1;P31=1;
    if(!P33)return 4;
    else if(!P32)return 5;
//    else if(!P31)return 6;
//    else if(!P30)return 7;
    P44=1;P42=0;
    if(!P33)return 8;
    else if(!P32)return 9;
//    else if(!P31)return 10;
//    else if(!P30)return 11;
    P42=1;P35=0;
    if(!P33)return 12;
    else if(!P32)return 13;
//    else if(!P31)return 14;
//    else if(!P30)return 15;
//    P35=1;P34=0;
//    if(!P33)return 16;
//    else if(!P32)return 17;
//    else if(!P31)return 18;
//    else if(!P30)return 19;
    return 0;
}

void Timer0Init(void)
{
    AUXR |= 0x80;
	TMOD &= 0xF0;		//��ʱ��ʱ��1Tģʽ
	TMOD |= 0x04;		//���ö�ʱ��ģʽ
	TL0 = 0;		//���ö�ʱ��ʼֵ
	TH0 = 0;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}
void PCA_TIME_init()
{
    P_SW1 &= 0xCF;//����PCA
    CCAPM0 = 0x11;
    PPCA=1;//������ȼ�
    CMOD = 0x00;
    CCON = 0x00;
    CH = 0;
    CL = 0;
    CCAP1L = TIME_1;
    CCAP1H = TIME_1>>8;
    CCAPM1 = 0x49;
    TIME_1 +=TIME_CCAP1;
    CCAP2L = TIME_1;
    CCAP2H = TIME_2>>8;
    CCAPM2 = 0x49;
    TIME_2 +=TIME_CCAP2;
    CR = 1;
}

void Timer1Init(void)		//
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x00;		//���ö�ʱ��ֵ
	TH1 = 0x00;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 0;		//��ʱ��1����ʼ��ʱ
    PCA_TIME_init();
}
