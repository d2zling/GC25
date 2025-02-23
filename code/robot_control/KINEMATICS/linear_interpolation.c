#include "linear_interpolation.h"

/*
    5阶线性插值算法
*/

char end_quene = 1;

/**
*@brief 求解五阶插值线性方程
*@param input： 五阶插值函数输入参数
*@return output_li5_t：五阶插值函数求解结果
*/
output_li5_t li5th_solve(input_li5_t input)
{
    output_li5_t output;
    float t = input.t1 - input.t0;
    float h = input.s1 - input.s0;

    output.a0 = input.s0;
    output.a1 = input.v0;
    output.a2 = 0.5f * input.a0;
    output.a3 = ((20.0f * h) - ((8.0f * input.v1 + 12.0f * input.v0) * t) - (3.0f * input.a0 - input.a1) * t * t) / (2.0f * t * t * t);
    output.a4 = ((-30.0f * h) + ((14.0f * input.v1 + 16.0f * input.v0) * t) + (3.0f * input.a0 - 2.0f * input.a1) * t * t) / (2.0f * t * t * t * t);
    output.a5 = ((12.0f * h) - (6.0f * (input.v1 + input.v0) * t) - (input.a1 - input.a0) * t * t) / (2.0f * t * t * t * t * t);

    output.t0 = input.t0;

    return output;
}

/**
*@brief 插值函数获取位移
*@param input： 五阶插值函数求解结果
*@return float：位移
*/
float get_displacement_from_li5(output_li5_t *output_solve, float now_time)
{
    float t = now_time - output_solve->t0;
    float _t = t;
    float s = 0.0f;

    s  = output_solve->a0;
    s += output_solve->a1 * _t;
    _t = _t * t;
    s += output_solve->a2 * _t;
    _t = _t * t;
    s += output_solve->a3 * _t;
    _t = _t * t;
    s += output_solve->a4 * _t;
    _t = _t * t;
    s += output_solve ->a5 * _t;

    return s;
}

/**
*@brief 插值函数获取速度
*@param input： 五阶插值函数求解结果
*@return float：速度
*/
float get_speed_from_li5(output_li5_t *output_solve, float now_time)
{
    float t = now_time - output_solve->t0;
    float _t = t;
    float v = 0.0f;

    v = output_solve->a1;
    v += 2.0f * output_solve->a2 * _t;
    _t = _t * t;
    v += 3.0f * output_solve->a3 * _t;
    _t = _t * t;
    v += 4.0f * output_solve->a4 * _t;
    _t = _t * t;
    v += 5.0f * output_solve->a5 * _t;

    return v;
}

/**
*@brief 插值函数获取加速度
*@param input： 五阶插值函数求解结果
*@return float：加速度
*/
float get_acceleration_from_li5(output_li5_t *output_solve, float now_time)
{
    float t = now_time - output_solve->t0;
    float _t = t;
    float a = 0.0f;

    a += 2.0f * output_solve->a2;
    a += 6.0f * output_solve->a3 * _t;
    _t = _t * t;
    a += 12.0f * output_solve->a4 * _t;
    _t = _t * t;
    a += 20.0f * output_solve->a5 * _t;

    return a;

}

/**
*@brief 自动解算列表
*@param node  节点列表
*@return void
*/
void solve_li5th_node(li5_t* node)
{
    input_li5_t input;
    char i = 0;

    while(1)
    {
        if(node[i+1].task == NULL)
        {
            input.t0 = node[i].time;
            input.s0 = node[i].dis;
            input.v0 = node[i].vel;
            input.a0 = node[i].acc;

            input.t1 = node[i+1].time;
            input.s1 = node[i+1].dis;
            input.v1 = node[i+1].vel;
            input.a1 = node[i+1].acc;

            node[i+1].solve = li5th_solve(input);
        }
        else if(*(char *)node[i+1].task == end_quene) break;

        i++;
    }

}
