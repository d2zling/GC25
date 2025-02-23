#ifndef __HWT101_DRIVER_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#include "delay.h"
#include "usart.h"
#include "dma.h"

#define __DISABLE           0
#define __ENABLE            1

#define buff_size           22

//陀螺仪功能选择
#define __HWT101_ACC        __DISABLE
#define __HWT101_DEG        __ENABLE

void hwt101_init(void);
void drift_calibration(void);
void set_out_put_speed(void);
void reset_angle(void);

extern float hwt101_acceleration_data;	
extern float abs_degree_data;

#endif
