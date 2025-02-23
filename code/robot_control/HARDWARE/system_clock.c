#include "system_clock.h"

/*
    此为系统时钟驱动文件，用于计时系统运行时间，以10ms为步进，
    10ms定时器中断服务函数中计时系统时间，
    并将系统时间以毫秒为单位存入全局变量sys_time_ms中。
*/

long long int sys_time_ms = 0;
u8 timer_10ms_flag = 0;

/**
 * @brief 定时器5初始化。10ms定时器
 * @param 无
 * @return 无
 */
void system_clock_init(void)
{
    timer_5_init(1000, 840);
}

/**
 * @brief 开始计时
 * @param 无
 * @return 无
 */
void start_count_time(sys_count_time_t *timer)
{
    timer->start_time = sys_time_ms;
    timer->count_now_time = 0;
}

/**
 * @brief 计时器
 * @param timer 计时器结构体
 * @return 1:计时完成 
 *         0:计时中
 */
char count_time(sys_count_time_t *timer)
{
    //计时完成
    if((timer->count_now_time) >= (timer->count_end_time))
    {
        return 1;
    }
    //计时
    else
    {
        timer->count_now_time = sys_time_ms - timer->start_time;
        return 0;
    }
}

/**
 * @brief 复位计时器，但是不暂停
 * @param timer 计时器结构体
 * @return 无
 */
void reset_timer(sys_count_time_t *timer)
{
    timer->count_now_time = 0;
}

/**
 * @brief 获取系统运行多少秒
 * @param void
 * @return long long int 系统运行秒
 */
long long int get_seconds(void)
{
    return sys_time_ms/1000;
}

/**
 * @brief 获取系统运行多少分钟
 * @param void
 * @return long long int 系统运行分钟数
 */
long long int get_minutes(void)
{
    return sys_time_ms/60000;
}

/**
 * @brief 格式化输出系统运行多少时间
 * 以分秒的形式输出
 * @param void
 * @return long long int 
 *         time[0]系统运行分钟
 *         time[1]系统运行秒
 */
long long int* ms_2_minutes_seconds(void)
{
    static long long int time[2];
    time[0] = sys_time_ms/60000;
    time[1] = (sys_time_ms%60000)/1000;
    return time;
}

/**
 * @brief 定时器5中断服务函数,计时系统时间，以10ms为步进
 * @param 无
 * @return 无
 */
void TIM5_IRQHandler(void)//每次产生一个中断溢出事件就会进入一次中断函数
{
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)//检测是否为溢出中断
    {
		sys_time_ms+=10;
		timer_10ms_flag = 1;
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);//清除中断标志位，否则启动时会进入中断服务函数
    }
}

