#ifndef __SIM800C_H__
#define __SIM800C_H__	 
#include "user_config.h"

#define SIM800C_PWRKEY GPIO->PADIRBCR.Word = 0x10000000;GPIO->PADIR.Word |= 0x0A000000;GPIO->PAINEB.Word &= 0xF5FFFFFF
#define PWRKEY_H GPIO->PADATABSR.Word  = 0x10000000
#define PWRKEY_L GPIO->PADATABCR.Word  = 0x10000000
#define SIM800C_STATUS 	GPIO->PAPORT.PORT_25
#define NET_LED GPIO->PAPORT.PORT_27


#define  GSM_MSG_STOP_FLAG  0x1A

extern uint8_t  serverIP[];

void sim800c_OFF(void);
void sim800c_init(uint32_t BPS);
ErrorStatus GSM_SMS_RCV(void);
void get_IMEI(void);
void GSM_TCPC_INIT(void);
ErrorStatus TCP_Connected(void);
ErrorStatus GSM_TCP_Connect(void);
void TCP_send(uint32_t serialnum);
ErrorStatus TCP_Recieve(void);

#endif


