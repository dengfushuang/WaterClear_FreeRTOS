/******************************************************************************************
* Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
* �ļ���:  system_HR8P506.c
* ��  ��:  ESM Application Team in
* ��  ��:  V1.00
* ��  ��:  2015/10/27
* ��  ��:  HR8P506ϵ��оƬ��ʼ���ļ�
* ��  ע:  ������HR8P506ϵ��оƬ
*          ���������ѧϰ����ʾʹ�ã����û�ֱ�����ô����������ķ��ջ������е��κη������Ρ�
*******************************************************************************************/

#include "system_HR8P506.h"
#include "HR8P506.h"
#include "lib_scu.h"
#include "lib_scs.h"
#include "lib_gpio.h"
#include "lib_wdt.h"

uint32_t SystemCoreClock = 16000000;

void SystemInit(void)
{
	SYSTICK_InitStruType sys;
	WDT_InitStruType wdt;
//	SCU_SysClkSelect(SCU_CLK_HRC);
//	PLLClock_Config(Enable,SCU_PLL_IN16M,SCU_PLL_48M,Enable);
//	sys.SysTick_ClkSource = SysTick_ClkS_Base;
//	sys.SysTick_ITEnable = Enable;
//	sys.SysTick_Value = 48000;
//	SysTick_Init(&sys);
	WDT_RegUnLock();
	wdt.WDT_Tms = 900;
	wdt.WDT_IE = Disable;
	wdt.WDT_Rst = Enable;
	wdt.WDT_Clock = WDT_CLOCK_PCLK;
	WDT_Init(&wdt);
	WDT_Enable();
}

