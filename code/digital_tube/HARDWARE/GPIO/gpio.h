#ifndef __GPIO_H
#define __GPIO_H

#include "sys.h"
#include "delay.h"

//数码管GPIO定义
#define _8_LED_GPIO     GPIOA
#define A      			GPIO_Pin_0
#define B               GPIO_Pin_1
#define C               GPIO_Pin_2
#define D               GPIO_Pin_3
#define E               GPIO_Pin_4
#define F               GPIO_Pin_5
#define G               GPIO_Pin_6
#define DP              GPIO_Pin_7

#define _3_LED_GPIO     GPIOB       
#define A_DIG1          GPIO_Pin_0 
#define A_DIG2          GPIO_Pin_1 
#define A_DIG3          GPIO_Pin_2 
#define B_DIG1          GPIO_Pin_3 
#define B_DIG2          GPIO_Pin_4 
#define B_DIG3          GPIO_Pin_5 

#define all_reset       0xFF
#define all_set         0x00
#define show_1          0xF9
#define show_2          0xA4
#define show_3          0xB0
#define show_ERR        0x86

void _3_8_PIN_LED_Init(void);
void num_show(char data);
void LED_display(char *display_data, int delay_time);

#endif

