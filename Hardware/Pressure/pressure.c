#include "pressure.h"
void pressure_Init()
{
#ifdef PCB_V1_00
	GPIO->PADIR.Word = 0X00000018;
	GPIO->PAINEB.Word = 0xFFFFFFE7;
 #endif
#ifdef PCB_V1_01
	GPIO->PADIR.Word = 0X00000180;
	GPIO->PAINEB.Word = 0xFFFFEBFF;
 #endif
}
uint32_t get_Pressure()
{
	uint32_t pre;
	/**********—π¡¶º∆À„***********/
	return pre;
}
