/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *�ļ���:  lib_printf.h
 *��  ��:  ESM Application Team 
 *��  ��:  V1.01
 *��  ��:  2016/11/01
 *��  ��:  Printf���ܿ⺯��ͷ�ļ�
 *��  ע:  ������HRSDK-GDB-8P506
 ���������ѧϰ����ʾʹ�ã����û�ֱ�����ô����������ķ��ջ������е��κη������Ρ�
 **********************************************************/
#ifndef __LIBPRINTF_H__
#define __LIBPRINTF_H__

#include "lib_uart.h"
#include <stdio.h>
#include <type.h>

ErrorStatus UART_printf(UART_TypeDef *UARTx, uint8_t *Data, ...);

#endif
