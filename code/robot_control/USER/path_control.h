#ifndef __PATH_CONTROL_H
#define __PATH_CONTROL_H

#include "robot_arm_control.h"
#include "chassis_control.h"
#include "linear_interpolation.h"

#define out_type_speed          0
#define out_type_displacement   1
#define out_type_acceleration   2

typedef struct _path_quene_t
{
    li5_t* quene;           //路径队列
    sys_count_time_t timer; //当前路径节点计时器
    char p_node;            //当前路径节点

}path_quene_t;

float chassis_path_control(path_quene_t *li5_path, char choose_out_type);

#endif