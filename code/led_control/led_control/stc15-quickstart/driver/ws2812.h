#ifndef __WS2812_H
#define __WS2812_H

#include "GPIO.h"
#include "delay.h"

void WS2812_Init(void);
void WS2812_SendByte_all(char R, char G, char B);
void WS2812_SendByte_single(char R, char G, char B);
void loop_show(void);
void slowly_brighten_show(u8 brightness);

#endif