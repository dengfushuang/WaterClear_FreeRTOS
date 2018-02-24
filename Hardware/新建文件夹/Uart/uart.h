#ifndef __UART_H
#define __UART_H
#include "user_config.h"

#define RCV_BUF_LEN 220
#define SEND_BUF_LEN 120

extern uint8_t RCV_DATA_BUF[RCV_BUF_LEN];
extern uint8_t SEND_DATA_BUF[SEND_BUF_LEN];

extern uint8_t UART0Init(void);
extern void UART0Putch(uint8_t Data);       
extern void UART0Put_str(uint8_t *Data, uint16_t NByte);
extern void UART0Write_Str(uint8_t *Data);
extern void clear_Buffer(void);
ErrorStatus UART0_Recieve(void);
ErrorStatus deal_String(const char *str);
extern ErrorStatus check_ststus(uint8_t *sendstr,char *str,uint8_t resend);
#endif

