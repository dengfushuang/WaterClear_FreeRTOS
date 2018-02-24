#include "uart.h"

uint8_t RCV_DATA_BUF[RCV_BUF_LEN];
uint8_t SEND_DATA_BUF[SEND_BUF_LEN];
extern volatile uint8_t sys_CLK;
uint16_t RxCounter;

SemaphoreHandle_t xSemUART0;
uint8_t UART0Init(void)
{
	GPIO_InitSettingType g;
	UART_InitStruType u;
	/************TXD**************/
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func1;
	g.Dir = GPIO_Direction_Output;
	g.ODE = GPIO_ODE_Output_Disable;
	g.DS = GPIO_DS_Output_Strong;
	GPIO_Init(GPIO_Pin_A23,&g);
	
	/*************RXD*****************/
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func1;
	g.Dir = GPIO_Direction_Input;
	g.PUE = GPIO_PUE_Input_Disable;
	g.PDE = GPIO_PDE_Input_Disable;
	GPIO_Init(GPIO_Pin_A22,&g);
	
	u.UART_BaudRate = 115200;
	u.UART_ClockSet = UART_Clock_1;
	u.UART_RxMode = UART_DataMode_8;
	u.UART_RxPolar = UART_Polar_Normal;
	u.UART_StopBits = UART_StopBits_1;
	u.UART_TxMode = UART_DataMode_8;
	u.UART_TxPolar = UART_Polar_Normal;
	UART_Init(UART0,&u);
	UART_TBIMConfig(UART0,UART_TBIM_Byte);
	UART_RBIMConfig(UART0,UART_TBIM_Byte);
	UART_ITConfig(UART0,UART_IT_RB,Enable);
	NVIC_Init(NVIC_UART0_IRQn,NVIC_Priority_1,Enable);
	UART0_RxEnable();
	xSemUART0 = xSemaphoreCreateBinary();
	return 1;
}
void UART0Putch(uint8_t Data)
{
	while(UART0->STA.TXBUSY == 1);
	UART0->TBW.Byte[0] = Data;
}

void UART0Put_str(uint8_t *Data, uint16_t NByte)
{
	UART0_TxEnable();
    while (NByte-- > 0)
    {
        UART0Putch(*Data++);
    }
	while(UART0->STA.TXBUSY == 1);
	UART0_TxDisable();
}

void UART0Write_Str(uint8_t *Data)
{
	UART0_TxEnable();
    while (*Data != '\0' )
    {
        UART0Putch(*Data++);
    }
	while(UART0->STA.TXBUSY == 1);
	UART0_TxDisable();
}

void clear_Buffer(void)
{
	uint16_t i;
	for( i =0 ; i < (RCV_BUF_LEN-1) ;i++)
	{
		RCV_DATA_BUF[i] = 0;
	}
	for( i =0 ; i < (SEND_BUF_LEN-1) ;i++)
	{
		SEND_DATA_BUF[i] = 0;
	}
	RxCounter = 0;
}
ErrorStatus UART0_Recieve(void)
{
	ErrorStatus err = ERROR;
	if(xSemaphoreTake( xSemUART0, 0x186A00UL ) == pdTRUE)
	{
		err = SUCCESS;
	}
	return err;
}
void UART0_IRQHandler()
{
	static BaseType_t tt;
	if(UART0->IF.RBIF)
	{
		RCV_DATA_BUF[RxCounter++] = UART_RecByte(UART0);
	}
	if(!UART0->STA.RXBUSY)
	{
		xSemaphoreGiveFromISR(xSemUART0,&tt);
		portYIELD_FROM_ISR(xSemUART0);
	}
}


ErrorStatus deal_String(const char *str)
{
	ErrorStatus err1 = ERROR;
	if(UART0_Recieve() == SUCCESS)
	{
		if(strstr((const char *)RCV_DATA_BUF,str) != NULL)
		{
			err1 = SUCCESS;
		}
	}
	return err1;
}
ErrorStatus check_ststus(uint8_t *sendstr,char *str,uint8_t resend)
{
	uint8_t i;
	ErrorStatus err0 = ERROR;
	clear_Buffer();
	for(i = 0 ; i < (resend+1) ; i++)
	{
		UART0Write_Str(sendstr);
		if(deal_String(str) == SUCCESS)
		{
			err0 = SUCCESS;
			break;
		}
		vTaskDelay(1000);
	}
	return err0;
}


