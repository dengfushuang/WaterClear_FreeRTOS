/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_flashiap.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  FlashIAP库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_flashiap.h"
#include "user_config.h"
/*************************************
  函数名：IAP_Unlock
  描  述：解锁IAP
  输入值：无
  返回值：成功/失败
 **************************************/
ErrorStatus IAP_Unlock(void)
{
	uint16_t i;

	IAP_RegUnLock();
	IAP_Enable();
	IAP_REQ();
	for (i = 0; i < 0xffff; ++i) {
		if (IAP->CON.FLASH_ACK != 0)
			break;
	}

	return i == 0xffff ? ERROR : SUCCESS;
}

/*************************************
  函数名：IAP_WriteEnd
  描  述：写Flash结束
  输入值：无
  返回值：成功/失败
 **************************************/
ErrorStatus IAP_WriteEnd(void)
{
    uint32_t i;

    IAP_RegUnLock();
    IAP_REQ_END();
    for (i = 0; i < 0xffff; ++i) {
        if (IAP->CON.FLASH_ACK == 0)
            break;
    }

    return i == 0xffff ? ERROR : SUCCESS;
}

/*************************************
  函数名：IAP_ErasePage
  描  述：页擦除
  输入值：页地址
  返回值：成功/失败
 **************************************/
ErrorStatus IAP_ErasePage(uint8_t Page_Addr)
{
	uint16_t i;
	uint32_t tmp;

	tmp = __get_PRIMASK();
	CPSID_i();
   //	__ASM("CPSID i");
	//__disable_irq();
	if ((IAP_Unlock()) == ERROR) {
		__set_PRIMASK(tmp);
		return ERROR;
	}

	IAP->ADDR.IAPPA = Page_Addr;
	IAP->TRIG.Word  = 0x00005EA1;

	for (i = 0; i < 0xffff; ++i) {
		if (IAP->STA.BSY == 0)
			break;
	}

	if (i == 0xffff)
		goto end;

	for (i = 0; i < 0xffff; ++i) {
		if (IAP->STA.ERASE_END == 1)
			break;
	}

	if (i == 0xffff)
		goto end;

	if ((IAP_WriteEnd()) == ERROR)
		goto end;

	__set_PRIMASK(tmp);
	return SUCCESS;

end:
	__set_PRIMASK(tmp);
	//__ASM("CPSIE i");
	CPSIE_i();
	return ERROR;
}

/*************************************
  函数名：IAP_WriteCont
  描  述：Flash连续写（内部调用）
  输入值：页内地址
  页地址
  要写入的数据
  返回值：成功/失败
 **************************************/
ErrorStatus IAP_WriteCont(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32)
{
	uint16_t i;

	IAP->ADDR.IAPPA = Page_addr;
	IAP->ADDR.IAPCA = Unit_addr;

	IAP->DATA.Word  = Data32;
	IAP->TRIG.Word  = 0x00005DA2;

	for (i = 0; i < 0xffff; ++i) {
		if (IAP->STA.BSY == 0)
			break;
	}

	if (i == 0xffff)
		return ERROR;

	for (i = 0; i < 0xffff; ++i) {
		if (IAP->STA.PROG_END)
			break;
	}

	if (i == 0xffff){
		return ERROR;
	}

	return SUCCESS;
}

/*************************************
  函数名：IAP_WriteWord
  描  述：Flash写
  输入值：页内地址
  页地址
  要写入的数据
  返回值：成功/失败
 **************************************/
ErrorStatus IAP_WriteWord(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32)
{
	uint32_t tmp;

	tmp = __get_PRIMASK();
	//__asm__("CPSID i");
	CPSID_i();
	//__disable_irq();
	if ((IAP_Unlock()) == ERROR)
		goto end;

	if ((IAP_WriteCont(Unit_addr, Page_addr, Data32)) == ERROR)
		goto end;

	if ((IAP_WriteEnd()) == ERROR)
		goto end;

	__set_PRIMASK(tmp);
	return SUCCESS;

end:
	__set_PRIMASK(tmp);
	//__ASM("CPSIE i");
	CPSIE_i();
	return ERROR;
}

/*************************************
  函数名：IAP_Read
  描  述：Flash读
  输入值：内存地址--读取数据的存放地址
  Flash地址
  读取数据的长度，单位为字（4字节）
  返回值：成功/失败
 **************************************/
ErrorStatus IAP_Read(uint32_t *Ram_Addr, uint32_t Flash_Addr, uint8_t Len)
{
	uint8_t i;
	uint32_t *ram_addr;
	const uint32_t *flash_addr;

	ram_addr = Ram_Addr;
	flash_addr = (const uint32_t *)Flash_Addr;

	if (Len == 0)
		return ERROR;

	for (i = 0; i < Len; ++i) {
		*ram_addr = *flash_addr;
		++ram_addr;
		++flash_addr;
	}

	return SUCCESS;
}
ErrorStatus Save_To_EPROM(void* data,uint16_t count)
{
    uint16_t address_temp,i;
	uint32_t *cp;
	
	cp = (uint32_t *)data;
    address_temp = (unsigned int* )data  - &EEPROM_BASE_ADDR;
    address_temp  = address_temp/4;
	for(i = 0 ; i < count ; )
	{
		if(IAP_WriteWord(address_temp+i,PAGE_ADDR,*cp) == SUCCESS)
		{
			i++;
			cp++;
		}
	}
	return SUCCESS;
}

