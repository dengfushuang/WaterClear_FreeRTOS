#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

//#define PCB_V1_01

/*******************************************************************
                       系统头文件
********************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "HR8P506.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "croutine.h"
#include "semphr.h"

#include "type.h"
#include "system_HR8P506.h"

/*******************************************************************
                       库头文件
********************************************************************/
#include "lib_gpio.h"
#include "lib_uart.h"
#include "lib_timer.h"
#include "lib_scs.h"
#include "lib_wdt.h"
#include "lib_adc.h"
#include "lib_flashiap.h"

/*******************************************************************
                       自定义头文件
********************************************************************/
#include "uart.h"
#include "tds.h"
#include "my_systick.h"
#include "sim800c.h"
#include "msgbuild.h"
#include "pressure.h"
#include "water_flow.h"

#define START_ADDR 0x00008800
#define PAGE_ADDR (START_ADDR/1024)

#define EEPROM_BASE_ADDR EPROM.ARRY[0]

#define DEBUG_T 0


typedef struct{
	uint32_t ServerFlow;
	uint32_t RunFlow;
	uint32_t ServerTime;
	uint32_t RunTime;
    uint32_t  ServerType;
    uint32_t  ValveStatus;
	uint32_t  CircleTime;
	uint32_t  ContinuTime;
    uint8_t IMEI[16];
	uint8_t ServerIP[32];
}EPROM_DATA;

typedef union{
    EPROM_DATA EPROM_S;
	uint32_t ARRY[12];
}EEPROM_DATA;

extern volatile uint8_t task_flag;
extern void my_Task_Timer_Init(void);
extern EEPROM_DATA EPROM;
extern void reset(void);
extern void write_All_Flash(void);
extern void read_All_Flash(void);
extern void BEE_ON(void);
extern void BEE_OFF(void);

#endif
