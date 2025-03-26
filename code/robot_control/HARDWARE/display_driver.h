#ifndef __DISPLAY_DRIVER_H
#define __DISPLAY_DRIVER_H

#include "gpio.h"
#include "delay.h"

#define DS(x)       GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)x)
#define STCP(x)     GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)x)
#define SHCP(x)     GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)x)
#define MR(x)       GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)x)

void display_init(void);
void _74hc595_write_bit(uint8_t bit);
void disp_num(void);

#endif