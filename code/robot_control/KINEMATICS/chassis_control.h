#ifndef __CHASSIS_CONTROL_H
#define __CHASSIS_CONTROL_H

#include "motor_driver.h"
#include "hwt101_driver.h"

#include "arm_math.h"

#define x_error_correction  0.0f
#define y_error_correction  0.0f
#define degree_pid_limit    100.0f

//单位mm
#define wheel_diameter      82.0f

void chassis_control_init(void);

void set_car_speed_Mecanum_Wheel(int x, int y, int w);
void chassis_control_Mecanum_Wheel(int x, int y, float degree);
void stop_Mecanum_Wheel(void);

void set_car_speed_Omni_Wheel(int x, int y, int w);
void chassis_control_Omni_Wheel(float x, float y, float degree);
void stop_Omni_Wheel(void);

#endif