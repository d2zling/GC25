#ifndef __LINEAR_INTERPOLATION_H
#define __LINEAR_INTERPOLATION_H

#include "system_clock.h"

#include <stdlib.h>

typedef struct _input_li5_t
{
    float t0; //开始时间
    float s0; //开始位移
    float v0; //开始速度
    float a0; //开始加速度

    float t1; //结束时间
    float s1; //结束加速度
    float v1; //结束速度
    float a1; //结束加速度

}input_li5_t;


typedef struct _output_li5_t
{
    //解算出五阶方程的7个参数
    float t0;
    float a0;
    float a1;
    float a2;
    float a3;
    float a4;
    float a5;

}output_li5_t;

typedef struct _li5_t
{
    float time;                     //节点时间
    float dis;                      //节点位移
    float vel;                      //节点速度
    float acc;                      //节点加速度

    void * task;                    //节点任务

    output_li5_t solve;             //5阶计算解算结果
}li5_t;

extern char end_quene;

output_li5_t li5th_solve(input_li5_t input);

float get_displacement_from_li5(output_li5_t *input_solve, float now_time);
float get_speed_from_li5(output_li5_t *input_solve, float now_time);
float get_acceleration_from_li5(output_li5_t *input_solve, float now_time);

void solve_li5th_node(li5_t* node);

#endif
