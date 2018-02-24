#ifndef __UART_H
#define __UART_H
#include "user_config.h"

#define RCV_BUF_LEN 220
#define SEND_BUF_LEN 120

extern uint8_t RCV_DATA_BUF[RCV_BUF_LEN];
extern uint8_t SEND_DATA_BUF[SEND_BUF_LEN];
extern uint8_t left_bracket,right_bracket;

extern void clear_RCV_Buffer(void);


extern uint8_t UART0Init(void);
extern uint8_t UART1Init(void);
extern void UART0Putch(uint8_t Data);
extern void UART1Putch(uint8_t Data);
extern void UART0Put_str(uint8_t *Data, uint16_t NByte);
extern void UART1Put_str(uint8_t *Data, uint16_t NByte);
extern void UART0Write_Str(uint8_t *Data);
extern void UART1Write_Str(uint8_t *Data);
extern uint8_t UART0Getch(void); 


extern ErrorStatus UART0_Recieve(uint32_t tout);
extern ErrorStatus get_MSG(char * str,uint32_t tout);
extern ErrorStatus get_String(uint8_t *sendstr,uint8_t resend);
extern ErrorStatus get_NByte(uint8_t *sendstr,uint16_t NByte);
extern ErrorStatus check_ststus(uint8_t *sendstr,const char *str,uint8_t resend,uint32_t tout);
#endif

