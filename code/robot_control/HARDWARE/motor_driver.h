#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H

#include "stm32f4xx.h"
#include "delay.h"
#include "arm_math.h"

#include "usart.h"
#include "timer.h"
#include "dma.h"

//电机控制结构体
typedef struct 
{
	char ID;			//电机ID
	char direction;	    //电机运动方向
	char synchronous;	//同步设定
	int speed;		    //速度
	int acc_speed;	    //加速度
	int pulse_num;	    //总脉冲数

} steeping_motor;

extern steeping_motor motor[7];

void motor_set_zero_point(steeping_motor _motor);
void motor_turn_to_zero(steeping_motor _motor);

void chassis_motor_init(void);
void chassis_motor_control_speed(steeping_motor _motor);
void chassis_motor_control_synchronous(void);

void gimbal_motor_init(void);
void gimbal_motor_control_speed(steeping_motor _motor);
void gimbal_motor_control_position(steeping_motor _motor);

void get_motor_angle(steeping_motor _motor);
void motor_pow_control(steeping_motor _motor, char state);

#endif
