#include "user_config.h"
#define CIRCLE_TIME 12

uint8_t  serverIP[] = "\"140.143.187.125\",\"11222\"\r\n";

const size_t xStackSize = 128;
TimerHandle_t MY_Timer ;
extern uint8_t beeflag ;

void vWDTTask( void *pvParameters );
void vTCPTask( void *pvParameters );
void vAPPTask( void *pvParameters );
void vTimTask( void *pvParameters );

void vTimerCallback( TimerHandle_t xTimer );

volatile uint32_t seconds = 0,minutes = 1,TDS1Time = 1,TDS2Time = 0;
EEPROM_DATA EPROM;



/*********************************************************************************************************
** 函数名称: BEE_init，BEE_ON，BEE_OFF
** 功能描述:蜂鸣器相关函数
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void BEE_init()
{
	GPIO_InitSettingType g;
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func0;
	g.Dir = GPIO_Direction_Output;
	g.ODE = GPIO_ODE_Output_Disable;
	g.DS = GPIO_DS_Output_Normal;
	GPIO_Init(GPIO_Pin_A1,&g);
	GPIO_WriteBit(GPIO_Pin_A1,0);
}
void BEE_ON()
{
	beeflag = 1;
}
void BEE_OFF()
{
	beeflag = 0;
}
/*********************************************************************************************************
** 函数名称: reset
** 功能描述:复位函数
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void reset()
{
	PWRKEY_L;
	Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
	hard_Delaynms(5000);
	Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
	hard_Delaynms(1500);
	PWRKEY_H;
	hard_Delaynms(5000);
	hard_Delaynms(5000);
	hard_Delaynms(5000);
//	NVIC_SystemReset();
}
void read_All_Flash()
{
	IAP_Read((uint32_t*)&EEPROM_BASE_ADDR,START_ADDR,(sizeof(EEPROM_DATA)/sizeof(uint32_t)));
	delay_nms(200);
	IAP_Read((uint32_t*)&EEPROM_BASE_ADDR,START_ADDR,(sizeof(EEPROM_DATA)/sizeof(uint32_t)));
}
void write_All_Flash()
{
	uint8_t i = 0;
	for(i = 0 ; i < 10 ;i++)
	{
		IAP_Enable();
		IAP_Unlock();
		IAP_ErasePage(PAGE_ADDR);
		delay_nms(5);
		if(Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)))== SUCCESS)
		{
			break;
		}
	}
}

/*********************************************************************************************************
** 函数名称: HardWare_Init
** 功能描述: 硬件初始化
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void HardWare_Init()
{
	BEE_init();
	valve_Init();
	tds_Init();
	UART0Init();
	UART1Init();
}

/*********************************************************************************************************
** 函数名称: main
** 功能描述:主函数
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
int main(void)
{
	SystemInit();
	HardWare_Init();
	
	xTaskCreate(vWDTTask,"WDT",xStackSize,NULL,2,NULL);
	xTaskCreate(vAPPTask,"APP",xStackSize,NULL,3,NULL);
	xTaskCreate(vTimTask,"Tim",xStackSize,NULL,4,NULL);
	xTaskCreate(vTCPTask,"TCP",xStackSize,NULL,5,NULL);
	
	vTaskStartScheduler();
	for(;;);
	return 0;
}
/*********************************************************************************************************
** 函数名称: vWDTTask
** 功能描述:看门狗任务
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void vWDTTask( void *pvParameters )
{
	uint8_t f = 1,err_count,connected_flag;
	while(1)
	{
		WDT_Clear();
		if(f == 1)
		{
			f = 0;
		}
		else
		{
			f = 1;
		}
		vTaskDelay(500);
	}
}
/*********************************************************************************************************
** 函数名称: vTCPTask
** 功能描述:TCP连接任务，用于网络连接
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void vTCPTask( void *pvParameters )
{
	uint8_t connected_flag = 0,i;
	uint32_t err_count = 0;
	read_All_Flash();
	if(EPROM.EPROM_S.CircleTime > 120)
	{
		EPROM.EPROM_S.CircleTime = 60;
	}
    for(err_count = 0 ;err_count < 5 ;err_count ++)
	{
		if(EPROM.EPROM_S.ServerIP[err_count] <32 || EPROM.EPROM_S.ServerIP[err_count] > 127)
		{
			connected_flag ++;
		}
	}
	if(connected_flag != 0 )
	{
		sprintf((char *)&EPROM.EPROM_S.ServerIP[0],"%s",(char *)serverIP);
	}
	sim800c_init(115200);
	while(1)
	{
		msg_Build(RCV_DATA_BUF);
		connected_flag = 0;
		GSM_SMS_RCV();
		connected_flag = 0;
		if(TCP_Connected() == ERROR)
		{
			GSM_TCPC_INIT();
			if(GSM_TCP_Connect() == SUCCESS)
			{ 
				connected_flag = 1;
			}
			else
			{
				err_count++;
			}
			if(err_count > 5)
			{
				GSM_SMS_RCV();
				reset();
			}
		}
		else
		{
			connected_flag = 1;
		}
		if(connected_flag)
		{
//			EPROM.EPROM_S.ValveStatus = 0x07; //测试语句，下载时注释掉
			connected_flag = 0;
			err_count = 0;
			clear_RCV_Buffer();
			msg_Build(RCV_DATA_BUF);
			if(TCP_Recieve() == SUCCESS)
			{
				msg_Deal(RCV_DATA_BUF);
			}
			else
			{
				err_count ++;
			}
		}
		for(i = 0 ; i < 15 ;i ++)
		{
			vTaskDelay(1000);
		}
	}
}
/*********************************************************************************************************
** 函数名称: vAPPTask
** 功能描述:数据采集任务，流量，TDS值
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void vAPPTask( void *pvParameters )
{
	TDS1Time = 1;
	TDS2Time = 0;
	while(1)
	{
		if(minutes%29 == 0)
		{
			Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
			delay_nms(70);
			Save_To_EPROM((uint32_t *)&EEPROM_BASE_ADDR,(sizeof(EPROM_DATA)/sizeof(uint32_t)));
		}
		runApplication();
    	control_Function();
		vTaskDelay(300);
	}
}
/*********************************************************************************************************
** 函数名称: vTimTask
** 功能描述:定时器任务，用于创建定时器
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void vTimTask( void *pvParameters )
{
	MY_Timer = xTimerCreate("Mytimer1",1000,pdTRUE,( void * ) 0,vTimerCallback);
    if( MY_Timer != NULL ) 
	{
		 if(xTimerStart(MY_Timer, 0) != pdPASS)
		 {
			 
		 }
    }
	while(1)
	{
		vTaskDelay(1000);
	}
}
/*********************************************************************************************************
** 函数名称: vTimerCallback
** 功能描述:定时器回调函数，用于秒级计数
** 输　入: 无
** 输　出: 无
********************************************************************************************************/
void vTimerCallback( TimerHandle_t xTimer )
{
	static uint32_t ulCount ;
	seconds++;
	UART1Write_Str((uint8_t *)"timer task\r\n");
	EPROM.EPROM_S.RunTime++;
	if(seconds>=60)
	{
		seconds = 0;
		minutes++;
	}
	if(minutes >= 60)
	{
		minutes = 0;
	}
	if(TDS1Time != 0)
	{
		TDS1Time ++;
	}
	if(TDS2Time != 0)
	{
		TDS2Time ++;
	}
}



