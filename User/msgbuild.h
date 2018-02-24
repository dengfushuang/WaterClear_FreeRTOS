#ifndef __MSG_BUILD_H
#define __MSG_BUILD_H
#include "user_config.h"
extern volatile uint32_t seconds,minutes,TDS1Time,TDS2Time;


void pressure_Init(void);
uint8_t high_Pressure(void);
uint8_t low_Pressure(void);
void valve_Init(void);
void valve_ON(uint8_t t);
void valve_OFF(uint8_t t);
extern void msg_Build(uint8_t *sendStr);
extern ErrorStatus msg_Deal(uint8_t *rcv);
extern void runApplication(void);
extern void control_Function(void);

#endif
