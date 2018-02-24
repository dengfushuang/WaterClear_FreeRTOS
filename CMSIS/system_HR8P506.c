/******************************************************************************************
* Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
* 文件名:  system_HR8P506.c
* 作  者:  ESM Application Team in
* 版  本:  V1.00
* 日  期:  2015/10/27
* 描  述:  HR8P506系列芯片初始化文件
* 备  注:  适用于HR8P506系列芯片
*          本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
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

