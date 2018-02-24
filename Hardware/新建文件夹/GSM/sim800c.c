#include "sim800c.h"

/***GSM SMS***/
uint8_t  GSM_BUF0[] = "AT\r\n";
uint8_t  GSM_BUF1[] = "AT+CMGF=0\r\n";
uint8_t  GSM_BUF3[] = "AT+CMGL=\"REC UNREAD\"\r\n";
uint8_t  GSM_BUF4[] = "AT+CNMI=\"GSM\"\r\n";
uint8_t  GSM_BUF5[] = "AT+CMGF=1\r\n";
/***GSM TCP***/
uint8_t  GSM_BUF6[] = "AT+CSTT=\"CMNET\"\r\n";
uint8_t  GSM_BUF7[] = "AT+CIICR\r\n";
uint8_t  GSM_BUF8[] = "AT+CIFSR\r\n";
uint8_t  CIPSTART[46] = "AT+CIPSTART=\"TCP\",";
uint8_t  GSM_CIPSRIP[] = "AT+CIPSRIP=1\r\n";
uint8_t  GSM_CIPSEND[] = "AT+CIPSEND\r\n";
uint8_t  GSM_CIPCLOSE[] = "AT+CIPCLOSE\r\n";
uint8_t  GSM_CIPSHUT[] = "AT+CIPSHUT\r\n";

void sim800c_OFF(void)
{
#ifdef PCB_V1_01
	PWRKEY_L;
	delay_nms(2000);
	PWRKEY_H;
	while(!SIM800C_STATUS)
	{
		delay_nms(10);
	}
	delay_nms(1000);
	PWRKEY_L;
	delay_nms(2000);
#endif
}
/*********************************************************************************************************
** ��������: sim800c_init
** ��������: GSMģ���ʼ��
** �䡡��: ��
** �䡡��: ��
********************************************************************************************************/
void sim800c_init(uint32_t BPS)
{
	volatile ErrorStatus err;
	uint8_t errcnt = 0;
	/***SIM800C��Դ���ƽų�ʼ��***/
	strcat((char *)CIPSTART,(char *)EPROM.EPROM_S.ServerIP);
    SIM800C_PWRKEY;
	delay_nms(200);
	PWRKEY_H;
	delay_nms(3000);
#ifdef PCB_V1_01
	while(SIM800C_STATUS)
	{
		delay_nms(10);
	}
#endif
	errcnt = 0;
REPARE:
	UART0Write_Str(GSM_BUF0);
	delay_nms(100);
	if(get_MSG("Ready",2000) == ERROR)
	{
		errcnt++;
		if(errcnt < 5)
		{
	    	goto REPARE;
		}
	}
	delay_nms(5000);  //ȷ��GSMģ��������������ٽ���ϵͳ
	BEE_ON();
	delay_nms(1000);
	BEE_OFF();
	clear_RCV_Buffer();
	UART0Write_Str(GSM_BUF0);
	delay_nms(1000);
	delay_nms(100);
	errcnt = 0;
	if(check_ststus(GSM_BUF1,"OK",10) == ERROR)
	{
		reset();
	}
	if(check_ststus((uint8_t *)"AT+CMGDA=6\r\n","OK",5) == ERROR)
	{
		errcnt++;
	}
	if(check_ststus(GSM_BUF5,"OK",5) == ERROR)
	{
        errcnt++;
	}
	if(check_ststus((uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n","OK",5) == ERROR)
	{
        errcnt++;
	}
	if(errcnt != 0)
	{
		reset();
	}
	delay_nms(5000); 
	delay_nms(3000);
	UART0Write_Str(GSM_CIPSRIP);
	delay_nms(2000);
	get_IMEI();
	delay_nms(1000);
}



ErrorStatus GSM_SMS_RCV(void)
{
	uint8_t *cp;
	uint8_t  data[35] = {0};
	uint8_t *cp_s,*cp_e;
	/***��ȡδ������***/
		delay_nms(10);
		UART0Write_Str(GSM_BUF5);
		delay_nms(10);
		UART0Write_Str(GSM_BUF4);
		delay_nms(10);
//		UART0Write_Str(GSM_BUF3);
		/***��ȡ������ָ������***/
	    if(check_ststus(GSM_BUF3,"+CMGL:",0) == ERROR)
		{
			return ERROR;
		}
        UART0_RxDisable();
	    UART0_TxDisable();	
		if((cp = (uint8_t *)strstr((char *)RCV_DATA_BUF, "+86")) == NULL)
		{
			return ERROR;
		}
		cp_s = (uint8_t *)strstr((char *)RCV_DATA_BUF,"<");
		cp_e = (uint8_t *)strstr((char *)RCV_DATA_BUF,">");
		if(cp_s == NULL || cp_e==NULL)
		{
			UART0Write_Str((uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n");
			delay_nms(100);
			return ERROR;
		}
		strncpy((char *)data,(char *)(cp_s+1),(strlen((char *)(cp_s+1))-strlen((char *)(cp_e-1))+1));
		sprintf((char *)&EPROM.EPROM_S.ServerIP[0],"%s\r\n",data);
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		delay_nms(70);
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		delay_nms(100);
	    UART0_RxEnable();
	    UART0_TxEnable();
		reset();
		return SUCCESS;
}


/*********************************************************************************************************
** ��������: get_IMEI
** ��������: ��ȡGSMģ��Ψһ��ʶ��
** �䡡��: ��
** �䡡��: ��
********************************************************************************************************/
void get_IMEI(void)
{
	uint16_t i = 0;
	int temp;
	uint8_t *cp;
	temp  = get_NByte((uint8_t *)"AT+GSN\r\n",17); 
	if(temp == ERROR)
	{
		reset();
	}
	delay_nms(200);
	i = 0;
	/***Ŀ����Ϣ***/
	cp = RCV_DATA_BUF;
	while(i < (RCV_BUF_LEN -1))
	{
		if( *cp >= '0' && *cp <= '9')
		{
			break;
		}
		cp++;
		i++;
	}
	if( *cp >= '0' && *cp <= '9')
	{
		for(i=0;i<15;i++)
		{
			EPROM.EPROM_S.IMEI[i] = *cp;
			cp++;
		}
		EPROM.EPROM_S.IMEI[15] = '\0';
	}
	delay_nms(200);
}
/*********************************************************************************************************
** ��������: GSM_TCPC_INIT
** ��������: GSM TCP�ͻ��˳�ʼ��
** �䡡��: ��
** �䡡��: ��
********************************************************************************************************/
void GSM_TCPC_INIT(void)
{
	
	delay_nms(3000);
	UART0Write_Str(GSM_CIPSRIP);
	delay_nms(2000);
}
ErrorStatus TCP_Connected(void)
{
	if(check_ststus((uint8_t *)"AT+CIPSTATUS\r\n","CONNECT OK",0,5000) == SUCCESS)
	{
		return SUCCESS;
	}
	return ERROR;
}
/*********************************************************************************************************
** ��������: GSM_TCP_Connect
** ��������: GSM TCP����
** �䡡��: ��
** �䡡��: ���سɹ�SUCCESS��ʧ��ERROR
********************************************************************************************************/
ErrorStatus GSM_TCP_Connect(void)
{
	volatile ErrorStatus temp = ERROR;
	static uint8_t err_count = 0;
	if(check_ststus(GSM_BUF6,"OK",0) == ERROR)
	{
		err_count ++;
	}
	if((temp = check_ststus(GSM_BUF7,"OK",0)) == ERROR)
	{
	    err_count ++;
	}
	if(err_count > 10)
	{
		err_count = 0;
		reset();
	}
	if(temp == SUCCESS)
	{
		    err_count =0;
			UART0Write_Str(GSM_BUF8);
			delay_nms(800);
			temp = check_ststus(CIPSTART,"CONNECT OK",0);
			if(temp == SUCCESS)
			{
				;
			}
			else
			{
				UART0Write_Str(GSM_CIPSHUT);
				delay_nms(10);
				temp = ERROR;
			}
	}
	else
	{
			UART0Write_Str(GSM_CIPCLOSE);
			delay_nms(100);
			UART0Write_Str(GSM_CIPSHUT);
			delay_nms(1000);
		    temp = ERROR;
	}
    return temp;
}




/*********************************************************************************************************
** ��������: TCP_Recieve
** ��������: GSM TCP��������
** �䡡��: ��
** �䡡��: ���سɹ�SUCCESS��ʧ��ERROR
********************************************************************************************************/
ErrorStatus TCP_Recieve(void)
{
	uint8_t i;
	ErrorStatus err0;
	left_bracket = 0;
	right_bracket = 0;
	UART0Write_Str(GSM_CIPSEND);
	delay_nms(100);
	UART0Write_Str(RCV_DATA_BUF);
	UART0Putch(GSM_MSG_STOP_FLAG);
	err0 = get_MSG("RECV FROM:",8000);
	if( err0 == SUCCESS)
	{
		i = 0;
		left_bracket = 0;
		right_bracket = 0;
		while((i < (RCV_BUF_LEN -1))&&(RCV_DATA_BUF[i]!= 0))
		{
			if( RCV_DATA_BUF[i] == '{')
			{
				left_bracket ++;
			}
			if(RCV_DATA_BUF[i] == '}')
			{
				right_bracket ++;
			}
			i++;
			if((left_bracket == right_bracket) && (left_bracket != 0))
			{
				return SUCCESS;
			}
		}
		UART0Write_Str(RCV_DATA_BUF);
	}
    return ERROR;		
}

