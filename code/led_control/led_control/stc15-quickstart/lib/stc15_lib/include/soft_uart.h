
#ifndef __SOFT_UART_H
#define __SOFT_UART_H

#include "config.h"

void TxSend(u8 dat);
void PrintString(unsigned char code *puts);

#ifdef DEBUG
#include <STDIO.H>
void LOG(const char *str, ...);
#else
#define LOG(x)
#endif

#endif
