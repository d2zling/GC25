#include "robot_arm_control.h"

/*
    机械臂控制
*/

static float degree_switch = 0.0f;              //实际角度转换电机脉冲系数
static float rad_2_angle = 0.0f;                //弧度转角度系数
static float angle_2_rad = 0.0f;                //角度转弧度系数
static float square_first_arm_lenght= 0.0f;    //大臂长度平方
static float square_second_arm_lenght= 0.0f;   //小臂长度平方

/**
*@brief 机械臂控制初始化
*@param 无
*@return 无
*/
void robot_arm_control_init(void)
{
    servo_init();
    gimbal_motor_init();
    motor_turn_to_zero(motor[4]);
    motor_turn_to_zero(motor[5]);
    motor_turn_to_zero(motor[6]);

    //常数参数计算
    rad_2_angle = 180.0f / PI;
    angle_2_rad = PI / 180.0f;
    degree_switch = 3200.0f / 360.0f * reduction_ratio;
    square_first_arm_lenght = first_arm_lenght * first_arm_lenght;
    square_second_arm_lenght = second_arm_lenght * second_arm_lenght;
}

/**
*@brief 机械臂底座控制
*@param degree: 云台绝对角度
*@return 无
*/
void robot_arm_control_botton_position(float degree)
{
    motor[5].acc_speed = 100;
    if(degree < 0)
    {
        motor[5].direction = 0;
        degree = -degree;
    }
    else
    {
        motor[5].direction = 1;
    }

    motor[5].pulse_num = (int)(degree * degree_switch + 0.5f);
    motor[5].speed = 100;

    gimbal_motor_control_position(motor[5]);
}

/**
*@brief 机械大小臂控制
*@param first_arm_degree: 大臂相对角度
*@param second_arm_degree: 小臂相对角度
*@return 无
*/
void robot_arm_control_arm_poosition(float first_arm_degree, float second_arm_degree)
{
    //根据机械臂实际情况转换角度
    first_arm_degree = 90.0f - first_arm_degree;
    second_arm_degree = 90.0f - second_arm_degree + first_arm_degree;
    //printf("first_arm_degree:%f, second_arm_degree:%f\r\n", first_arm_degree, second_arm_degree);

    if(first_arm_degree < 0)
    {
        motor[6].direction = 1;
        first_arm_degree = -first_arm_degree;
    }
    else
    {
        motor[6].direction = 0;
    }

    if(second_arm_degree < 0)
    {
        motor[4].direction = 0;
        second_arm_degree = -second_arm_degree;
    }
    else
    {
        motor[4].direction = 1;
    }

    motor[6].pulse_num = (int)(first_arm_degree * degree_switch + 0.5f);
    motor[6].speed = 150;

    motor[4].pulse_num = (int)(second_arm_degree * degree_switch + 0.5f);
    motor[4].speed = 150;

    gimbal_motor_control_position(motor[4]);
    gimbal_motor_control_position(motor[6]);
}

/**
*@brief yz平面机械臂运动学逆解算
*@param y: y轴运动距离
*@param z: z轴运动距离
*@param out_arm_degree: 输出机械臂角度
*@return void
*/
void robot_arm_calculate_inverse(float y, float z, float* out_arm_degree)
{
    float   first_arm_degree,       //大臂角度
            second_arm_degree,      //小臂角度
            first_arm_rad_1,        //大臂弧度计算中间值1
            first_arm_rad_2,        //大臂弧度计算中间值2
            short_side,             //短边
            hypotenuse;             //斜边   

    //考虑爪子长度高度,修正坐标系
    y = y - gripper_lenght; 
    z = z + gripper_height;
    if(y < 0) y = 0;

    //计算斜边
    short_side = base_height - z;
    if(short_side < 0) short_side = short_side * -1.0f;
    arm_sqrt_f32(short_side * short_side + y * y, &hypotenuse);

    //计算大臂中间值1弧度
    first_arm_rad_1 = acos((square_first_arm_lenght  -
                            square_second_arm_lenght + 
                            hypotenuse * hypotenuse) /
                            (2.0f * first_arm_lenght * hypotenuse));

    //计算大臂中间值2弧度
    //bug:有 -PI/2.0f 输出数据才正常
    if(base_height > z) first_arm_rad_2 = atan(y / short_side) - PI / 2.0f;
    else if(base_height < z) first_arm_rad_2 = atan(short_side / y);
    else first_arm_rad_2 = 0.0f;

    //大臂角度计算
    first_arm_degree = (first_arm_rad_1 + first_arm_rad_2) * rad_2_angle;

    //小臂角度计算
    second_arm_degree = acos((square_first_arm_lenght  +
                              square_second_arm_lenght -
                              hypotenuse * hypotenuse) /
                              (2.0f * first_arm_lenght * second_arm_lenght)) * 
                              rad_2_angle;

    out_arm_degree[0] = first_arm_degree;   //大臂角度
    out_arm_degree[1] = second_arm_degree;  //小臂角度
}

/**
*@brief 机械臂运动学正解算
*@param first_arm_degree: 大臂角度
*@param second_arm_degree: 小臂角度
*@param out_arm_location_yz: 输出坐标
*@return void
*/
float* robot_arm_calculate_forward(float first_arm_degree, float second_arm_degree, float* out_arm_location_yz)
{
    float z1, z2, y1, y2;                                   //大臂和小臂的z轴和y轴增量

    second_arm_degree = second_arm_degree + first_arm_degree;

    first_arm_degree = first_arm_degree - 90.0f;                            //计算第一段机械臂在坐标系的旋转角度
    second_arm_degree = - 180.0f + second_arm_degree + first_arm_degree;    //计算第二段机械臂在坐标系的旋转角度

    z1 = first_arm_lenght * arm_cos_f32(first_arm_degree * angle_2_rad);
    y1 = first_arm_lenght * arm_sin_f32(first_arm_degree * angle_2_rad);
                                                                                            
    z2 = second_arm_lenght * arm_cos_f32(second_arm_degree * angle_2_rad);
    y2 = second_arm_lenght * arm_sin_f32(second_arm_degree * angle_2_rad);
       
    out_arm_location_yz[0] = y1 + y2 + gripper_lenght;
    out_arm_location_yz[1] = z1 + z2 - gripper_height + base_height;

    return out_arm_location_yz;
}
