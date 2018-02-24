/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_flashiap.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  FlashIAP库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBIAP_H__
#define __LIBIAP_H__

#include "HR8P506.h"
#include "type.h"

/* Register UnLock */
#define IAP_RegUnLock()		(IAP->UL.Word = 0x000000A5)
#define IAP_RegLock()		(IAP->UL.Word = 0x0)

/* IAP Enable */
#define IAP_Enable()		(IAP->CON.EN = 1)
#define IAP_Disable()		(IAP->CON.EN = 0)

/* Flash IAP request */
#define IAP_REQ()		(IAP->CON.FLASH_REQ = 1)
#define IAP_REQ_END()	(IAP->CON.FLASH_REQ = 0)


/* IAP Function */
ErrorStatus IAP_Unlock(void);
ErrorStatus IAP_WriteEnd(void);
ErrorStatus IAP_ErasePage(uint8_t Page_Addr);
ErrorStatus IAP_WriteCont(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32);
ErrorStatus IAP_WriteWord(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32);
ErrorStatus IAP_Read(uint32_t *Ram_Addr, uint32_t Flash_Addr, uint8_t Len);
ErrorStatus Save_To_EPROM(void* data,uint16_t count);
#endif
