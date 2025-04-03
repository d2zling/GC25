#ifndef __USART_H
#define __USART_H
#include "stdio.h"

extern char display_data[6];

void uart1_Init(u32 bound);
void rec_data(u8 data);

#endif

