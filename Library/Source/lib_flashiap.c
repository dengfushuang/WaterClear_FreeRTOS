/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *�ļ���:  lib_flashiap.c
 *��  ��:  ESM Application Team 
 *��  ��:  V1.01
 *��  ��:  2016/11/01
 *��  ��:  FlashIAP�⺯��
 *��  ע:  ������HRSDK-GDB-8P506
 ���������ѧϰ����ʾʹ�ã����û�ֱ�����ô����������ķ��ջ������е��κη������Ρ�
 **********************************************************/
#include "lib_flashiap.h"
#include "user_config.h"
/*************************************
  ��������IAP_Unlock
  ��  ��������IAP
  ����ֵ����
  ����ֵ���ɹ�/ʧ��
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
  ��������IAP_WriteEnd
  ��  ����дFlash����
  ����ֵ����
  ����ֵ���ɹ�/ʧ��
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
  ��������IAP_ErasePage
  ��  ����ҳ����
  ����ֵ��ҳ��ַ
  ����ֵ���ɹ�/ʧ��
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
  ��������IAP_WriteCont
  ��  ����Flash����д���ڲ����ã�
  ����ֵ��ҳ�ڵ�ַ
  ҳ��ַ
  Ҫд�������
  ����ֵ���ɹ�/ʧ��
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
  ��������IAP_WriteWord
  ��  ����Flashд
  ����ֵ��ҳ�ڵ�ַ
  ҳ��ַ
  Ҫд�������
  ����ֵ���ɹ�/ʧ��
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
  ��������IAP_Read
  ��  ����Flash��
  ����ֵ���ڴ��ַ--��ȡ���ݵĴ�ŵ�ַ
  Flash��ַ
  ��ȡ���ݵĳ��ȣ���λΪ�֣�4�ֽڣ�
  ����ֵ���ɹ�/ʧ��
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

