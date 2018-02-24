#include "msgbuild.h"
#define CIRCLE_COUNT 5

volatile uint32_t tds1 = 0,tds2 = 0;

/*********************************************************************************************************
** 函数名称: pressure_Init
** 功能描述:压力管脚初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void pressure_Init(void)
{


	GPIO->PADATA.Word = 0x00000180;
	GPIO->PAINEB.Word &= 0xFFFFFE7F;
	GPIO->PADIR.Word  = 0X00000180;

}
/*********************************************************************************************************
** 函数名称: high_Pressure
** 功能描述:高压开关
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
uint8_t high_Pressure(void)
{

   
	   if(GPIO->PAPORT.PORT_7)
	   {
		   return 1;
	   }
	   else
	   {
		   return 0;
	   }
}
/*********************************************************************************************************
** 函数名称: low_Pressure
** 功能描述:低压开关
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
uint8_t low_Pressure(void)
{
	
	   if(GPIO->PAPORT.PORT_8)
	   {
		   return 1;
	   }
	   else
	   {
		   return 0;
	   }
}
/*********************************************************************************************************
** 函数名称: valve_Init
** 功能描述:阀门管脚初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void valve_Init(void)
{
	GPIO_InitSettingType g;
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func0;
	g.Dir = GPIO_Direction_Output;
	g.ODE = GPIO_ODE_Output_Disable;
	g.DS = GPIO_DS_Output_Normal;
	GPIO_Init(GPIO_Pin_A11,&g);
	GPIO_Init(GPIO_Pin_A12,&g);
	GPIO_Init(GPIO_Pin_A13,&g);
	GPIO_WriteBit(GPIO_Pin_A11,0);
	GPIO_WriteBit(GPIO_Pin_A12,0);
	GPIO_WriteBit(GPIO_Pin_A13,0);
}
/*********************************************************************************************************
** 函数名称: valve_ON，valve_OFF
** 功能描述:阀门开关
** 输　入: 0全部阀门，1第一个阀门，2第二个阀门，3第三阀门
** 输　出: 无
********************************************************************************************************/
void valve_ON(uint8_t t)
{
	switch(t)
	{
		case 0:GPIO_WriteBit(GPIO_Pin_A11,1);GPIO_WriteBit(GPIO_Pin_A12,1);GPIO_WriteBit(GPIO_Pin_A13,1);break;
		case 1:GPIO_WriteBit(GPIO_Pin_A11,1);break;
		case 2:GPIO_WriteBit(GPIO_Pin_A12,1);break;
	    case 3:GPIO_WriteBit(GPIO_Pin_A13,1);break;
		default:break;
	}
}
void valve_OFF(uint8_t t)
{
	switch(t)
	{
		case 0:GPIO_WriteBit(GPIO_Pin_A11,0);GPIO_WriteBit(GPIO_Pin_A12,0);GPIO_WriteBit(GPIO_Pin_A13,0);break;
		case 1:GPIO_WriteBit(GPIO_Pin_A11,0);break;
		case 2:GPIO_WriteBit(GPIO_Pin_A12,0);break;
	    case 3:GPIO_WriteBit(GPIO_Pin_A13,0);break;
		default:break;
	}
}
/*********************************************************************************************************
** 函数名称: get_Valve_Status
** 功能描述:获取阀门状态
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
uint8_t get_Valve_Status()
{
	uint8_t ret = 0;
	ret = (GPIO->PAPORT.Word&0x00003800)>>11;
	return ret;
}
/*********************************************************************************************************
** 函数名称: type_Time
** 功能描述:时间服务
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void type_Time()
{
	/**************服务时间到**************/
	if(EPROM.EPROM_S.RunTime >= EPROM.EPROM_S.ServerTime)
	{
		if(minutes % 5 == 1)
		{
//			BEE_ON();
//			delay_nms(5000);
//			delay_nms(5000);
//			BEE_OFF();
		}
		valve_OFF(0);
		EPROM.EPROM_S.ValveStatus = 0;
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		delay_nms(70);
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
	}	
}
/*********************************************************************************************************
** 函数名称: type_Flow
** 功能描述:流量服务
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void type_Flow()
{
	/**************服务流量到**************/
	if(EPROM.EPROM_S.RunFlow >= EPROM.EPROM_S.ServerFlow)
	{
		
		if(minutes % 5 == 1)
		{
//			BEE_ON();
//			delay_nms(5000);
//			delay_nms(5000);
//			BEE_OFF();
		}
		valve_OFF(0);
		EPROM.EPROM_S.ValveStatus = 0;
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		delay_nms(70);
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
	}
}
/*********************************************************************************************************
** 函数名称: msg_Build
** 功能描述:发送的信息
** 输　入: 发送数据的数组
** 输　出: 无
********************************************************************************************************/
void msg_Build(uint8_t *sendStr)
{
	uint16_t len;
	char temp[50];
	if(EPROM.EPROM_S.ServerType != 0 || EPROM.EPROM_S.ServerType != 1)
	{
		EPROM.EPROM_S.ServerType = 0;
	}
	len = sprintf((char *)sendStr,"{\"SN\":\"%s\",",(char *)&EPROM.EPROM_S.IMEI[0]);
    len+=sprintf((char *)temp,"\"CMD\":\"01\",");
	strcat((char *)sendStr,temp);
	len+=sprintf((char *)temp,"\"TYPE\":%c,",EPROM.EPROM_S.ServerType+'0');
	strcat((char *)sendStr,temp);
	len+=sprintf((char *)temp,"\"VS\":\"%c%c%c\",",(((EPROM.EPROM_S.ValveStatus & 0x04)>> 2)+'0'),(((EPROM.EPROM_S.ValveStatus & 0x02) >> 1)+'0'),((EPROM.EPROM_S.ValveStatus & 0x01)+'0'));
	strcat((char *)sendStr,temp);
	len+=sprintf((char *)temp,"\"TDS1\":%u,\"TDS2\":%u,\"F\":%u}",tds1,tds2,EPROM.EPROM_S.RunFlow);
	strcat((char *)sendStr,temp);
}

/*********************************************************************************************************
** 函数名称: msg_Deal
** 功能描述:处理接收到的信息
** 输　入: 接收数据的数组
** 输　出: 无
********************************************************************************************************/
ErrorStatus msg_Deal(uint8_t *rcv)
{
	char *cp1,*cp2;
	uint8_t flag = 0,type = 0,vs = 0,t = 0,ft = 0;
	uint32_t sf = 0,rt = 0,et = 0;
	cp2 = (char *)rcv;
	WDT_Clear();
	if((cp1 = strstr(cp2,"TYPE")) != NULL)
	{
		type = *(cp1+6)-'0';
	}else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"VS")) != NULL)
	{
		vs = 0;
		cp1+=5;
		if(*(cp1+0) == '1')
		{
			vs += 4;
		}
		if(*(cp1+1) == '1')
		{
			vs += 2;
		}
		if(*(cp1+2) == '1')
		{
			vs += 1;
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"T")) != NULL)
	{
		cp1 +=3;
		t = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				t *=10;
				t +=*cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"FT")) != NULL)
	{
		cp1 +=4;
		ft = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				ft *=10;
				ft +=*cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"SF")) != NULL)
	{
		cp1 +=4;
		sf = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				sf *= 10;
				sf += *cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"RT")) != NULL)
	{
		cp1 +=4;
		rt = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				rt *= 10;
				rt += *cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"ET")) != NULL)
	{
		cp1 +=4;
		et = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				et *= 10;
				et += *cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	if(!flag)
	{
		//sprintf(&EPROM.EPROM_S.IMEI[0],"123456789abcdef");
		EPROM.EPROM_S.ServerType = type&0x000000FF;
		EPROM.EPROM_S.ValveStatus = vs&0x000000FF;
		EPROM.EPROM_S.CircleTime = t&0x000000FF;
		EPROM.EPROM_S.ContinuTime = ft&0x000000FF;
		EPROM.EPROM_S.ServerFlow = sf;
		EPROM.EPROM_S.RunTime = rt;
		EPROM.EPROM_S.ServerTime  = et;
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		delay_nms(10);
		Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}




/*********************************************************************************************************
** 函数名称: runApplication
** 功能描述: 数据采集，流量和TDS
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void runApplication()
{
	uint32_t waterflow;
	uint8_t tds_arry[40];
	if(EPROM.EPROM_S.ValveStatus & 0x04)
	{
		;
	}
	else
	{
		valve_OFF(1);
	}
	if(EPROM.EPROM_S.ValveStatus & 0x02)
	{
		;
	}
	else
	{
		valve_OFF(2);
	}
	if(EPROM.EPROM_S.ValveStatus & 0x01)
	{
		;
	}
	else
	{
		valve_OFF(3);
	}
	if(EPROM.EPROM_S.ServerType == 1)
	{
		type_Flow();
	}

	if(EPROM.EPROM_S.ServerType == 0)
	{
		type_Time();
	}
	waterflow = get_Flow();
	get_TDS(&tds1,&tds2);
	sprintf((char *)tds_arry,"tds1=%u,tds2=%u\r\n",tds1,tds2);
	UART1Write_Str(tds_arry);
	EPROM.EPROM_S.RunFlow = EPROM.EPROM_S.RunFlow+waterflow;
}
/*********************************************************************************************************
** 函数名称: control_Function
** 功能描述: 控制函数，逻辑控制
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void control_Function(void)
{
	static uint32_t start_open_time = 0;
	static uint8_t fflag = 1;
	if(EPROM.EPROM_S.ValveStatus == 0)
	{
		valve_OFF(1);
		valve_OFF(2);
		valve_OFF(3);
		fflag = 1;
//		BEE_OFF();	
	}else
	{
		if(low_Pressure())
		{
			valve_OFF(1);
			valve_OFF(2);
			valve_OFF(3);
			fflag = 1;
//			BEE_OFF();
		}else
		{
			if(high_Pressure())
			{
//				BEE_OFF();
				fflag = 1;
				valve_OFF(1);
				valve_OFF(2);
				valve_OFF(3);
			}else
			{
				
//				BEE_ON();
				if(fflag)
				{
					start_open_time = EPROM.EPROM_S.RunTime;
					fflag = 0;
				}
				valve_ON(3);
				valve_ON(2);
				valve_ON(1);
				if(tds2 < 200)
				{
					valve_OFF(2);
				}
			}
		}
	}
	if((EPROM.EPROM_S.RunTime -start_open_time) > EPROM.EPROM_S.ContinuTime)
	{
		valve_OFF(0);
	}
}
