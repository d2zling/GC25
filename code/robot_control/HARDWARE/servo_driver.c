#include "servo_driver.h"

/**
 * @brief 舵机初始化
 * @param void 
 * @return 无
 */
void servo_init(void)
{
    timer_4_init(20000 - 1, 84 - 1);
}

/**
 * @brief 设置爪舵机角度
 * @param float 舵机角度
 * @return 无
 */
void set_servo_angel(float angel)
{
    TIM_SetCompare4(TIM4, (uint32_t)((angel / 180 * 2000) + 500));
}

