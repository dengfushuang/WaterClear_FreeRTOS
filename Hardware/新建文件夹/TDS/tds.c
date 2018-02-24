#include "tds.h"
#define TIME_DELAY 10
extern TimerHandle_t MY_Timer;;
volatile uint32_t count1,count2;
volatile uint8_t tflag;
/*void timer_Init(void)
{
//	T16Nx_BaseInitStruType T;
//	
//	T.T16Nx_ClkS  = T16Nx_ClkS_PCLK;
//	T.T16Nx_EDGE = T16Nx_EDGE_Rise;
//	T.T16Nx_Mode = T16Nx_Mode_TC0;
//	T.T16Nx_PREMAT = 1;
//	T.T16Nx_SYNC = Disable;
//	T16Nx_BaseInit(T16N2,&T);
//	T16Nx_MAT0ITConfig(T16N2,T16Nx_Clr_Int);
//	T16Nx_SetCNT0(T16N2,0);
//	T16Nx_SetMAT0(T16N2,40000);
//	T16Nx_ITConfig(T16N2,T16Nx_IT_MAT0,Enable);
//	NVIC_Init(NVIC_T16N2_IRQn,NVIC_Priority_1,Enable);
	
	;
}*/
ErrorStatus start_counter()
{
	
	if(MY_Timer != NULL)
	{
		count1 = 0;
		count2 = 0;
		WDT_Disable();
		T16Nx_Enable(T16N0);
		T16Nx_Enable(T16N1);
		xTimerStart(MY_Timer,0);
		return SUCCESS;
	}else
	{
		return ERROR;
	}
	
}
void tds_Init(void)
{
	T16Nx_BaseInitStruType T;
	GPIO_InitSettingType g;
	
	T.T16Nx_ClkS  = T16Nx_ClkS_CK0;
	T.T16Nx_EDGE = T16Nx_EDGE_Fall;
	T.T16Nx_Mode = T16Nx_Mode_TC0;
	T.T16Nx_PREMAT = 1;
	T.T16Nx_SYNC = Disable;
	T16Nx_BaseInit(T16N0,&T);
	
	T.T16Nx_ClkS  = T16Nx_ClkS_CK0;
	T.T16Nx_EDGE = T16Nx_EDGE_Fall;
	T.T16Nx_Mode = T16Nx_Mode_TC0;
	T.T16Nx_PREMAT = 1;
	T.T16Nx_SYNC = Disable;
	T16Nx_BaseInit(T16N1,&T);
	
	
	g.Func = GPIO_Reuse_Func3;
	g.Dir = GPIO_Direction_Input;
	g.ODE = GPIO_ODE_Output_Disable;
	g.DS = GPIO_DS_Output_Normal;
	g.PUE = GPIO_PUE_Input_Disable;
	g.PDE = GPIO_PDE_Input_Disable;
	g.Signal = GPIO_Pin_Signal_Digital;
    GPIO_Init(GPIO_Pin_A14,&g);
	GPIO_Init(GPIO_Pin_A16,&g);
	
	T16Nx_MAT0ITConfig(T16N0,T16Nx_Go_No);
	T16Nx_MAT0ITConfig(T16N1,T16Nx_Go_No);
	T16Nx_SetCNT0(T16N0,0);
	T16Nx_SetCNT0(T16N1,0);
	T16Nx_SetMAT0(T16N0,1);
	T16Nx_SetMAT0(T16N1,1);
	T16Nx_ITConfig(T16N0,T16Nx_IT_MAT0,Enable);
	T16Nx_ITConfig(T16N1,T16Nx_IT_MAT0,Enable);
	NVIC_Init(NVIC_T16N0_IRQn,NVIC_Priority_2,Enable);
	NVIC_Init(NVIC_T16N1_IRQn,NVIC_Priority_2,Enable);
}
ErrorStatus get_Fre1(volatile uint32_t *ct1,volatile uint32_t *ct2)
{
	tflag = 1;
	if(start_counter() == SUCCESS)
	{
		while(tflag);
		*ct1 = count1;
		*ct2 = count2;
		return SUCCESS;
	}else
	{
		return ERROR;
	}
	
}
ErrorStatus get_TDS(volatile uint32_t *tds1,volatile uint32_t *tds2)
{
	volatile uint32_t x1,x2;
	/***********¼ÆËãË®ÖÊ**************/
	if(get_Fre1(&x1,&x2)==SUCCESS)
	{
		*tds1 = x1;
		*tds2 = x2;
		return SUCCESS;	
	}
	else
	{
		return ERROR;
	}
	
	
}
void T16N0_IRQHandler(void)
{
	
	count1++;
}
void T16N1_IRQHandler(void)
{
	count2++;
}
void vTimeCallback(TimerHandle_t xTimer)
{
	static uint32_t uCount;
	uCount = (uint32_t)pvTimerGetTimerID(xTimer);
	++uCount;
	vTimerSetTimerID(xTimer,(void *)uCount);
	if(uCount >= TIME_DELAY)
	{
		xTimerStop( xTimer, 0 );
		T16Nx_Disable(T16N0);
		T16Nx_Disable(T16N1);
		T16Nx_Disable(T16N2);
		WDT_Clear();
		tflag = 0;
	}
	
}

