/********************************** (C) COPYRIGHT *******************************
 * File Name          : Concurrent_for_CT107S
 * Author             : 0x49181f36
 * Version            : V1.0.1
 * Date               : 2024/03/30
 * Description        :Concurrent .
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/
Changelog
data:2024/03/30
V1.0.1:Fixed a bug in keyboard program causing NE555 frequency to be low

data:2024/03/26
V1.0.0:Rebuilding
	   Completely updated the running logic
	   Added digital display interface for STC-ISP
	   Added software timer
	   Added PCA timer
	   Current timer schedule
	   TIM0 NE555 output
	   TIM1 ultrasound
	   TIM2 UART
	   CCAP0 ultrasound
	   CCAP1 systick
	   CCAP2 display

data:2023/06/06
V0.0.3:update "void Tim_0(void) interrupt 1" function
           Delete "SCON &= 0x02"
           update "uart.h" file
           Delete "void uart_Send(unsigned char len);"function declarations

data:2023/06/02
V0.0.2:update "unsigned char Key_Read()" function

data:2023/05/24
V0.0.1:Project publishing