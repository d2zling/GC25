#ifndef __KEY_H
#define __KEY_H

#include "GPIO.h"
#include "soft_uart.h"
#include "delay.h"

void key_Init(void);

extern char key_flag_0;
extern char key_flag_1;

#endif