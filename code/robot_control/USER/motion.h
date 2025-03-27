#ifndef __MOTION_H
#define __MOTION_H

#include "protocols.h"

#include "chassis_control.h"
#include "robot_arm_control.h"
#include "hwt101_driver.h"

//各种目标坐标位置
/*                             y         z          degree   */
#define rotator_num_1_position 338.260f, 170.053f,  12.203f
#define rotator_num_2_position 340.208f, 169.198f,  -15.460f
#define rotator_num_3_position 186.781f, 167.856f,  -2.381f
 
#define disk_hight             65.0f
#define disk_num_1_position    89.875f,  182.709f,  52.668f
#define disk_num_2_position    89.888f,  182.305f,  -0.102f
#define disk_num_3_position    85.526f,  182.780f,  -52.754f

#define ground_num_1_position  286.512f, 84.194f, 29.699f
#define ground_num_2_position  245.586f, 86.904f, 00.300f
#define ground_num_3_position  283.127f, 85.920f, -28.932f

#define ground_num_1_second_floor_position  325.175f, 154.950f, 29.666f
#define ground_num_2_second_floor_position  281.361f, 154.908f, 3.732f
#define ground_num_3_second_floor_position  301.924f, 154.994f, -23.307f

//定位死区
#define first_location_dead_area    5
#define second_location_dead_area   5
#define degree_location_dead_area   3

//圆盘定位目标位置
#define rotator_targe_x                 113
#define rotator_targe_y                 90

//物料区第一次定位目标位置
#define process_targe_x_first           135
#define process_targe_y_first           103

//物料区第二次定位目标位置
#define process_targe_x_second          160
#define process_targe_y_second          120

//二层粗定位目标
#define process_targe_x_first_height    167
#define process_targe_y_first_height    118

//二层精定位目标
#define process_targe_x_height          168
#define process_targe_y_height          101

//圆心定位限幅
#define circle_locatin_limit            2.0f

//中间位置动作
#define robot_arm_set_middle_position()         \
        robot_arm_control_botton_position(0.0f),\
        robot_arm_control_yz(210.0f, 280.0f)

#define robot_arm_reset_yz()                    \
        robot_arm_control_yz(79.464f, 250.833f)

void qr_code_task(void);
void grab_item_on_rotator_plus(uint8_t rotator_num);
void garb_item_on_guound_plus(uint8_t item_color);
void garb_item_on_disk_plus(uint8_t item_color);
void place_item_on_disk(uint8_t disk_num);
void place_item_on_ground(uint8_t item_color, char second_floor);

//定位
void rotator_location_task(void);
void processing_location_task(char enable_circle_location);

//任务
void rotator_task_plus(void);
void processing_task_plus(void);

extern float _abs_robot_arm_angle;

#endif