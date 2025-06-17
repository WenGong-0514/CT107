/********************************** (C) COPYRIGHT *******************************
 * File Name          : Concurrent_for_CT107S
 * Author             : 0x49181f36
 * Version            : V1.0.3
 * Date               : 2025/06/16
 * Description        :Concurrent .
 * Open Source License: GPL3.0
 * E-mail             : stm32f103@qq.com
 *******************************************************************************/
Changelog
date:2025/06/16
V1.0.3:Update serial port control function
	update Serial port automatic sending
	update Serial port idle automatic reception

date:2024/11/29
V1.0.2:Standardized variable names
	update"void PCF8591_Adc(unsigned char* ADC_Value)"function
	optimize EEPROM function

date:2024/03/30
V1.0.1:Fixed a bug in keyboard program causing NE555 frequency to be low
	
date:2024/03/26
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

date:2023/06/06
V0.0.3:update "void Tim_0(void) interrupt 1" function
           Delete "SCON &= 0x02"
           update "uart.h" file
           Delete "void uart_Send(unsigned char len);"function declarations

date:2023/06/02
V0.0.2:update "unsigned char Key_Read()" function

date:2023/05/24
V0.0.1:Project publishing