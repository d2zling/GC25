#ifndef __ROBOT_ARM_CONTROL_H
#define __ROBOT_ARM_CONTROL_H

#include "servo_driver.h"
#include "motor_driver.h"
#include "hwt101_driver.h"

#include "math.h"

//机械臂关节减速比
#define reduction_ratio     4.0f

//机械臂规格，单位为mm
#define base_height         92.3f //底座高度
#define first_arm_lenght    180.0f //第一个关节长度
#define second_arm_lenght   180.0f //第二个关节长度
#define gripper_lenght      0.0f   //机械爪长度
#define gripper_height      0.0f   //机械爪高度

#define robot_arm_pow_control(_motor,state)  motor_pow_control(_motor, (char)state)

void robot_arm_control_init(void);
void robot_arm_control_botton_position(float degree);
void robot_arm_control_botton_speed(float speed);
void robot_arm_control_botton_speed_CL(float targe);
void robot_arm_control_arm_poosition(float first_arm_degree, float second_arm_degree);
void robot_arm_control_yz(float y, float z);
void robot_arm_calculate_inverse(float y, float z, float* out_arm_degree);
float* robot_arm_calculate_forward(float first_arm_degree, float second_arm_degree, float* out_arm_location_yz);
void robot_arm_get_position(void);
void robot_arm_wait_stop(void);

#endif