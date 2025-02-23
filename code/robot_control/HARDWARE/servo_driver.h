#ifndef __SERVO_DRIVER_H
#define __SERVO_DRIVER_H

#include "timer.h"

//机械爪张开和闭合的角度
#define claw_close   150
#define claw_open    70
#define claw_open_small    100

#define open_claw()         set_servo_angel(claw_open)
#define open_claw_small()   set_servo_angel(claw_open_small)
#define close_claw()        set_servo_angel(claw_close)

void servo_init(void);
void set_servo_angel(float angel);

#endif