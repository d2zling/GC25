#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx.h"
#include "usart.h"

//void timer_2_init(uint16_t arr, uint16_t psc);
void timer_4_init(uint16_t arr, uint16_t psc);
void timer_5_init(uint16_t arr, uint16_t psc);

#endif