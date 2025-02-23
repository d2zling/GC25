/*
	不使用文件
*/
#include "remote_car_control.h"

/*  
    遥控工创车程序
*/

float degree = 0;

/**
 * @brief 遥控麦克纳姆轮车功能
 * @param 无
 * @return 无
 */
void remote_car_control_Mecanum_Wheel(void)
{
    float* data = get_remote_control_data();
    
    if(data != NULL)
    {
        //摇杆死区
        if(data[0] < 0.1f && data[0] > -0.1f) data[0] = 0;
        if(data[1] < 0.1f && data[1] > -0.1f) data[1] = 0;
        if(data[2] < 0.3f && data[2] > -0.3f) data[2] = 0;

        degree += data[2] * -3;

        chassis_control_Mecanum_Wheel(300*data[1], 300*data[0] * -1, degree);
    }
}
