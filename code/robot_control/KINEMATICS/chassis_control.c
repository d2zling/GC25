#include "chassis_control.h"

/*  
    底盘电机控制
*/

// static arm_pid_instance_f32 degree_pid;         //控制角度pid
static float angle_2_rad = 0.0f;                //角度转弧度系数
static float cos_wheel = 0.0f;                  //运动学解算三角函数值
static float circumference = 0.0f;              //轮子周长
/**
*@brief 底盘控制初始化
*@param 无
*@return 无
*/
void chassis_control_init(void)
{
    //硬件初始化
    chassis_motor_init();
    hwt101_init();

    delay_ms(500);
    set_out_put_speed();//设置陀螺仪输出速度
    delay_ms(500);   
    reset_angle();      //重置陀螺仪角度
    
    //软件初始化
    //常数参数计算
    angle_2_rad = PI / 180.0f;
    cos_wheel = arm_cos_f32(45.0f * angle_2_rad);
    circumference = wheel_diameter * PI;
}

/**
*@brief 轮子速度mm/ms转化为rpm
*@param 无
*@return 无
*/
float speed_mm_per_ms_to_rpm(float speed_mm_per_ms)
{
    return speed_mm_per_ms * 60000.0f / circumference;
}

/**
*@brief 底盘角度控制
*@param 无
*@return 无
*/
float chassis_control_degree(float target_degree)
{
    float err = abs_degree_data - target_degree;
    static float result;

    result = err * 3.0f;

    if(result > degree_pid_limit) result = degree_pid_limit;
    else if(result < -degree_pid_limit) result = -degree_pid_limit; 

    return result;
}

/**
*@brief 世界坐标转换为底盘坐标
*@param x 世界坐标x
*@param y 世界坐标y
*@return 转换后的坐标
*/
float* coordinate_transform(float x, float y)
{
    static float out[2];
    float out_cos, out_sin;

    out_cos = arm_cos_f32(abs_degree_data * angle_2_rad);
    out_sin = arm_sin_f32(abs_degree_data * angle_2_rad);

    out[0] = x * out_cos - y * out_sin;//x
    out[1] = x * out_sin + y * out_cos;//y

    return out;
}

/**
*@brief 设置麦克纳姆轮车辆速度
*@param x 前后速度
*@param y 左右速度
*@param w 旋转速度
*@return 无
*/
void set_car_speed_Mecanum_Wheel(int x, int y, int w)
{
	int motor_speed[4];

    //解算出每个电机的速度
	motor_speed[0] = x - y - w;
	motor_speed[1] = x + y + w;
	motor_speed[2] = x + y - w;
	motor_speed[3] = x - y + w;

	motor[0].direction = motor[2].direction = 1;
	motor[1].direction = motor[3].direction = 0;

    //方向控制
	for(char i = 0; i<4; i++)
	{
		if(motor_speed[i] < 0)
		{
			motor[i].direction = !motor[i].direction;
			motor_speed[i] = motor_speed[i] * -1;
		}
	}

	for(char i = 0; i<4; i++)
	{
		motor[i].speed = motor_speed[i];
		chassis_motor_control_speed(motor[i]);
	}
}

/**
*@brief 麦克纳姆轮单次底盘运动控制
*@param x 世界坐标x
*@param y 世界坐标y
*@param degree 角度
*@return 无
*/
void chassis_control_Mecanum_Wheel(int x, int y, float degree)
{
    float* speed_data;
    float abs_err_degree;

    abs_err_degree = abs_degree_data - degree;
    arm_abs_f32(&abs_err_degree, &abs_err_degree, 1);

    //TODO寻找更好的修正方案
    if(abs_err_degree > 10.0f) //角度相差大于10度时，修正运动
    {
        if(y == 0 && degree < 0)  y += y_error_correction;
        else if(y == 0 && degree > 0)  y -= y_error_correction;
        else if(x == 0 && degree < 0)  x += x_error_correction;
        else if(x == 0 && degree > 0)  x -= x_error_correction;
    }

    speed_data = coordinate_transform(x, y);

    //角度闭环死区 
    if(abs_err_degree < 0.4f)
        set_car_speed_Mecanum_Wheel((int)speed_data[0], (int)speed_data[1], 0);
    else 
        set_car_speed_Mecanum_Wheel((int)speed_data[0], (int)speed_data[1], chassis_control_degree(degree));
}

/**
*@brief  麦克纳姆轮底盘停车
*@param 无
*@return 无
*/
void stop_Mecanum_Wheel(void)
{
    set_car_speed_Mecanum_Wheel(0, 0, 0);
}

/**
*@brief 设置全向轮车辆速度
*@param x 前后速度,单位:mm/ms
*@param y 左右速度
*@param w 旋转速度
*@return 无
*/
void set_car_speed_Omni_Wheel(int x, int y, int w)
{
	motor[0].speed = - x + y - w;
	motor[1].speed = - x + y + w;
	motor[2].speed = x + y - w;
	motor[3].speed = x + y + w;

	motor[0].direction = motor[2].direction = 1;
	motor[1].direction = motor[3].direction = 0;

	for(char i = 0; i<4; i++)
	{
		if(motor[i].speed < 0)
		{
			motor[i].direction = !motor[i].direction;
			motor[i].speed = motor[i].speed * -1;
		}
	}

	for(char i = 0; i<4; i++)
	{
		chassis_motor_control_speed(motor[i]);
	}
}

/**
*@brief 全向轮轮单次底盘运动控制
*@param x 世界坐标x速度以mm为单位
*@param y 世界坐标y速度以mm为单位
*@param degree 角度
*@return 无
*/
void chassis_control_Omni_Wheel(float x, float y, float degree)
{
    static float* speed_data;
    static float abs_err_degree = 0;
    static float angle_speed = 0;

    abs_err_degree = abs_degree_data - degree;
    arm_abs_f32(&abs_err_degree, &abs_err_degree, 1);

    speed_data = coordinate_transform(x, y);
    angle_speed = chassis_control_degree(degree);

    speed_data[0] = speed_mm_per_ms_to_rpm(speed_data[0]) * cos_wheel;
    speed_data[1] = speed_mm_per_ms_to_rpm(speed_data[1]) * cos_wheel;

    set_car_speed_Omni_Wheel((int)speed_data[0], (int)speed_data[1], (int)angle_speed);

    //printf("d:%f, %f, %d, %d, %d\r\n", degree,  abs_degree_data, (int)speed_data[0], (int)speed_data[1], (int)angle_speed);
}

/**
*@brief  全向轮底盘停车
*@param 无
*@return 无
*/
void stop_Omni_Wheel(void)
{
    set_car_speed_Omni_Wheel(0, 0, 0);
}
