#include "uart.h"
uint8_t data_temp;
uint8_t RCV_DATA_BUF[RCV_BUF_LEN];
uint8_t SEND_DATA_BUF[SEND_BUF_LEN];
uint8_t left_bracket=0,right_bracket=0;
volatile uint8_t start_rcv_flag = 1;
volatile uint8_t rcv_head_flag = 1;
volatile uint8_t rcv_enf_flag = 1;
volatile uint8_t err_rcv_head_flag = 1;
volatile uint8_t RxCounter = 0,RxCounter1 = 0,RxByte = 0;
/*********************************************************************************************************
** ��������: UART0Init
** ��������: UART0��ʼ��
** �䡡��: ��
** �䡡��: ��
********************************************************************************************************/
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
	UART0_TxEnable();
	return 1;
}
uint8_t UART1Init(void)
{
	GPIO_InitSettingType g;
	UART_InitStruType u;
	/************TXD**************/
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func2;
	g.Dir = GPIO_Direction_Output;
	g.ODE = GPIO_ODE_Output_Disable;
	g.DS = GPIO_DS_Output_Strong;
	GPIO_Init(GPIO_Pin_B1,&g);
	
	/*************RXD*****************/
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func2;
	g.Dir = GPIO_Direction_Input;
	g.PUE = GPIO_PUE_Input_Disable;
	g.PDE = GPIO_PDE_Input_Disable;
	GPIO_Init(GPIO_Pin_B0,&g);
	
	u.UART_BaudRate = 115200;
	u.UART_ClockSet = UART_Clock_1;
	u.UART_RxMode = UART_DataMode_8;
	u.UART_RxPolar = UART_Polar_Normal;
	u.UART_StopBits = UART_StopBits_1;
	u.UART_TxMode = UART_DataMode_8;
	u.UART_TxPolar = UART_Polar_Normal;
	UART_Init(UART1,&u);
	UART_TBIMConfig(UART1,UART_TBIM_Byte);
	UART_RBIMConfig(UART1,UART_TBIM_Byte);
//	UART_ITConfig(UART1,UART_IT_RB,Enable);
//	NVIC_Init(NVIC_UART1_IRQn,NVIC_Priority_1,Enable);
//	UART1_RxEnable();
	UART1_TxEnable();
	return 1;
}
/*********************************************************************************************************
** ��������: clear_Buffer
** ��������: ������ջ���
** �䡡��: ��
** �䡡��: ��
********************************************************************************************************/
void clear_RCV_Buffer(void)
{
	uint16_t i;
	for( i =0 ; i < (RCV_BUF_LEN-1) ;i++)
	{
		RCV_DATA_BUF[i] = 0;
	}
}
/*********************************************************************************************************
** ��������: UART0Putch
** ��������: �������һ�ֽ�
** �䡡��: Data
** �䡡��: ��
********************************************************************************************************/
void UART0Putch(uint8_t Data)
{
	while(UART0->TB7.TBFF7 == 1);
	UART0->TBW.Byte[0] = Data;
}
void UART1Putch(uint8_t Data)
{
	while(UART1->TB7.TBFF7 == 1);
	UART1->TBW.Byte[0] = Data;
}
/*********************************************************************************************************
** ��������: UART0Put_str
** ��������: �������NByte�ֽ�
** �䡡��: ����Data������NByte
** �䡡��: ��
********************************************************************************************************/
void UART0Put_str(uint8_t *Data, uint16_t NByte)
{
    while (NByte-- > 0)
    {
        UART0Putch(*Data++);
    }
	while(UART0->STA.TXBUSY == 1);
}
void UART1Put_str(uint8_t *Data, uint16_t NByte)
{
    while (NByte-- > 0)
    {
        UART1Putch(*Data++);
    }
	while(UART1->STA.TXBUSY == 1);
}
/*********************************************************************************************************
** ��������: UART0Write_Str
** ��������: ��������ַ���
** �䡡��: �ַ���
** �䡡��: ��
********************************************************************************************************/
void UART0Write_Str(uint8_t *Data)
{
	RxCounter1 = 0;
	RxCounter = 0;
	start_rcv_flag = 1;
	rcv_head_flag = 1;
	err_rcv_head_flag = 1;
    while (*Data != '\0' )
    {
        UART0Putch(*Data);
		Data++;
    }
	while(UART0->STA.TXBUSY == 1);
}
void UART1Write_Str(uint8_t *Data)
{
    while (*Data != '\0' )
    {
        UART1Putch(*Data);
		Data++;
    }
	while(UART1->STA.TXBUSY == 1);
}
/*********************************************************************************************************
** ��������: UART0Getch
** ��������: ��ȡһ�ֽ�
** �䡡��: ��
** �䡡��: ��ȡ��������
********************************************************************************************************/
uint8_t UART0Getch(void)
{
    uint8_t err;
    err = UART_RecByte(UART0);
    return err;
}

/*********************************************************************************************************
** ��������: UART0_Recieve
** ��������:���ڽ��գ�
** �䡡��: ��
** �䡡��: �ɹ�SUCCESS,ʧ��ERROR
********************************************************************************************************/
ErrorStatus UART0_Recieve(uint32_t tout)
{
	uint32_t i = 0;
	RxCounter1 = 0;
	RxCounter = 0;
	start_rcv_flag = 1;
	rcv_head_flag = 1;
	err_rcv_head_flag = 1;
	while(start_rcv_flag)
	{
		i++;
		vTaskDelay(1);
		if(i>tout)
		{
			return ERROR;
		}    
	}
	i= 0;
    while((RxCounter == 0) || (UART0->STA.RXBUSY == 1))
	{
		i++;
		vTaskDelay(1);
		if(i>tout)
		{
			return ERROR;
		}
	}
	for(i = RxCounter ; i <(RCV_BUF_LEN -1);i++ )
	{
		RCV_DATA_BUF[i] = 0;
	}
	return SUCCESS;
}



/*********************************************************************************************************
** ��������: UART0_Recieve_String
** ��������:���ڽ��գ�
** �䡡��: �Ƚ��ַ���ͷ�����ո�������ʱʱ�䣨ms��
** �䡡��: �ɹ�SUCCESS,ʧ��ERROR
********************************************************************************************************/
/*ErrorStatus UART0_Recieve_String(char *headstr,uint8_t Nbyte,uint32_t tout)
{
	uint8_t i;
    if(headstr != NULL)
	{
		sprintf((char *)SEND_DATA_BUF,"%s",headstr);
	}
	RxCounter1 = 0;
	RxCounter = 0;
	rcv_head_flag = 1;
	start_rcv_flag = 1;
	rcv_msg_flag = 1;
	timeout = tout;
	do{
		
		if(timeout == 0)
		{
			return ERROR;
		}
	}while(start_rcv_flag);
	timeout = 1000;
    do{
		if(timeout == 0)
		{
			if(RxCounter != 0)
			{
				if(UART0->STA.RXBUSY == 0)
				{
					return SUCCESS;
				}
			}
			return ERROR;
		}
	}while(UART0->STA.RXBUSY);
	for(i = RxCounter ; i <(RCV_BUF_LEN -1);i++ )
	{
		RCV_DATA_BUF[i] = 0;
	}
	return SUCCESS;
}*/
/*********************************************************************************************************
** ��������: UART0_IRQHandler
** ��������: �����ж�
** �䡡��: ��
** �䡡��: ��
********************************************************************************************************/
void UART0_IRQHandler()
{
	
	if((UART_GetITStatus(UART0,UART_IT_RB) != RESET) && (UART_GetFlagStatus(UART0,UART_FLAG_RB) != RESET))
	{
		start_rcv_flag = 0;
		data_temp = UART0->RBR.Byte[0];
		if(rcv_head_flag)
		{
			if(data_temp == SEND_DATA_BUF[RxCounter1])
			{
				RxCounter1++;
				if(RxCounter1 >=(SEND_BUF_LEN -1) )
				{
					err_rcv_head_flag = 0;
				}
				else if((SEND_DATA_BUF[RxCounter1] == '\r') || (SEND_DATA_BUF[RxCounter1] == '\n') || (SEND_DATA_BUF[RxCounter1] == '\0'))
				{
					rcv_head_flag = 0;
				}
			}
			else
			{
				RxCounter1 = 0;
			}
		}else
		{
			if(data_temp >= 32 && data_temp <= 127)
			{
				RCV_DATA_BUF[RxCounter] = data_temp;
			    RxCounter++;
			}
			if(data_temp == '{')
			{
				left_bracket++;
			}
			if(data_temp == '}')
			{
				right_bracket++;
			}
			if((left_bracket == right_bracket ) && (left_bracket != 0))
			{
				rcv_enf_flag = 0;
			}
		}
	}
}




/*********************************************************************************************************
** ��������: get_MSG
** ��������: ���ڽ������ڱȽϴ����Ƿ���ܵ��ַ���str;
** �䡡��: �Ƚ��ַ���str
** �䡡��: �ɹ�SUCCESS,ʧ��ERROR
********************************************************************************************************/

ErrorStatus get_MSG(char * str,uint32_t tout)
{
	uint32_t i;
	sprintf((char *)SEND_DATA_BUF,"%s",str);
	RxCounter1 = 0;
	RxCounter = 0;
	start_rcv_flag = 1;
	rcv_head_flag = 1;
	err_rcv_head_flag = 1;
	rcv_enf_flag = 1;
	i = 0;
	do
	{
		/********��ʱ����*********/
		i++;
		vTaskDelay(1);
		if(i>tout)
		{
			return ERROR;
		}
	}while(start_rcv_flag);
	i = 0;
	do{
		i++;
		vTaskDelay(1);
		if(i > tout)
		{
			return ERROR;
		}
		if(err_rcv_head_flag == 0)
		{
			return ERROR;
		}
	}while(rcv_enf_flag);
    return SUCCESS;	
}
/*********************************************************************************************************
** ��������: get_String
** ��������: ��ȡ����sendstr�ַ���֮�󷵻ص���Ϣ��������Ϣͷ�������sendstr
** �䡡��: ������Ϣ���ظ����ʹ���
** �䡡��: ��
********************************************************************************************************/
ErrorStatus get_String(uint8_t *sendstr,uint8_t resend)
{
	uint8_t i,j;
	for(i = 0 ; i < (resend+1) ; i++)
	{
		for( j =0 ; j < (SEND_BUF_LEN-1) ;j++)
	    {
		    SEND_DATA_BUF[j] = 0;
	    }
	    sprintf((char *)SEND_DATA_BUF,"%s",(char *)sendstr);
		UART0Write_Str(SEND_DATA_BUF);
		if(UART0_Recieve(5000) == SUCCESS)
		{
			return SUCCESS;
		}
	}
	return ERROR;
}
/*********************************************************************************************************
** ��������: get_NByte
** ��������: ��ȡ����sendstr�ַ���֮�󷵻ص�NByte�ֽڣ�������Ϣͷ�������sendstr
** �䡡��: ������Ϣ���ظ����ʹ���
** �䡡��: ��
********************************************************************************************************/
ErrorStatus get_NByte(uint8_t *sendstr,uint16_t NByte)
{
	sprintf((char *)SEND_DATA_BUF,"%s",sendstr);
	UART0Write_Str(sendstr);
	RxCounter = 0;
	RxCounter1 = 0;
	RxCounter1 = 0;
	RxCounter = 0;
	start_rcv_flag = 1;
	rcv_head_flag = 1;
	err_rcv_head_flag = 1;
	start_rcv_flag = 1;
	while(start_rcv_flag);
	while(RxCounter < NByte);
	return SUCCESS;
}
/*********************************************************************************************************
** ��������: check_ststus
** ��������: ��ȡ����sendstr�ַ���֮�󷵻ص���Ϣ(ֻ������ӡ�ַ�)�����Ƚ��Ƿ����str�ַ���������Ϣͷ�������sendstr
** �䡡��: �����ַ���sendstr���Ƚ��ַ���str���ط�����resend
** �䡡��: ��
********************************************************************************************************/
ErrorStatus check_ststus(uint8_t *sendstr,const char *str,uint8_t resend,uint32_t tout)
{
	uint8_t i,j;
	ErrorStatus err;
	for(i = 0 ; i < (resend+1) ; i++)
	{
		for( j =0 ; j < (SEND_BUF_LEN-1) ;j++)
	    {
		    SEND_DATA_BUF[j] = 0;
	    }
	    sprintf((char *)SEND_DATA_BUF,"%s",(char *)sendstr);
		UART0Write_Str(SEND_DATA_BUF);
		err = UART0_Recieve(tout);
		if( err == SUCCESS)
		{
			delay_nms(200);
			if((strstr((const char *)RCV_DATA_BUF,str) != NULL))
			{
				return SUCCESS;
			}else
			{
				delay_nms(5000);
				if((strstr((const char *)RCV_DATA_BUF,str) != NULL))
			    {
				   return SUCCESS;
			    }
			}
		}
		clear_RCV_Buffer();
	}
	return ERROR;
}





