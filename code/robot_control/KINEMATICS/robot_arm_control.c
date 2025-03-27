#include "robot_arm_control.h"

/*
    机械臂控制
*/

static u8 robot_arm_motor_rx_data[30];          //电机返回数据缓存

static float degree_switch = 0.0f;              //实际角度转换电机脉冲系数
static float rad_2_angle = 0.0f;                //弧度转角度系数
static float angle_2_rad = 0.0f;                //角度转弧度系数
static float square_first_arm_lenght= 0.0f;    //大臂长度平方
static float square_second_arm_lenght= 0.0f;   //小臂长度平方

float gimbal_motor_1_position = 0;        //云台电机位置
float gimbal_motor_2_position = 0;        //云台电机位置
float gimbal_motor_3_position = 0;        //云台电机位置

/**
*@brief 机械臂控制初始化
*@param 无
*@return 无
*/
void robot_arm_control_init(void)
{
    servo_init();
    gimbal_motor_init();
    usart3_rx_dma_init(robot_arm_motor_rx_data, 20);

    robot_arm_pow_control(motor[4],ENABLE);
    robot_arm_pow_control(motor[5],ENABLE);
    robot_arm_pow_control(motor[6],ENABLE);

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
*@brief 机械臂底座角度控制
*@param degree: 云台绝对角度
*@return 无
*/
void robot_arm_control_botton_position(float degree)
{
    motor[5].acc_speed = 150;
    
    if(degree < 0)
    {
        motor[5].direction = 1;
        degree = -degree;
    }
    else
    {
        motor[5].direction = 0;
    }

    motor[5].pulse_num = (int)(degree * degree_switch + 0.5f);
    motor[5].speed = 300;

    gimbal_motor_control_position(motor[5]);
}

/**
*@brief 机械臂底座控制速度
*@param speed: 速度
*@return 无
*/
void robot_arm_control_botton_speed(float speed)
{
    if(speed < 0)
    {
        motor[5].direction = 0;
        speed = -speed;
    }
    else
    {
        motor[5].direction = 1;
    }

    motor[5].speed = (int)(speed + 0.5f);
    motor[5].acc_speed = 0;
    gimbal_motor_control_speed(motor[5]);
}

/**
*@brief 机械臂底座电机闭环控制速度
*@param targe: 机械臂保持的目标角度
*@return 无
*/
void robot_arm_control_botton_speed_CL(float targe)
{
    float abs_robot_arm_angle, err;

    get_motor_angle(motor[5]);

    abs_robot_arm_angle = gimbal_motor_2_position + abs_degree_data;

    // printf("%f %f %f\r\n", switch_angle, abs_degree_data, gimbal_motor_2_position);

    err = targe - abs_robot_arm_angle;

    printf("%f\r\n", err);

    err = err * -3.0f;

    if(err > 100) err = 100;
    else if(err < -100) err = -100;

    robot_arm_control_botton_speed(err);
}

/**
*@brief 机械大小臂角度控制
*@param first_arm_degree: 大臂相对角度
*@param second_arm_degree: 小臂相对角度
*@return 无
*/
void robot_arm_control_arm_poosition(float first_arm_degree, float second_arm_degree)
{
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
        motor[4].direction = 1;
        second_arm_degree = -second_arm_degree;
    }
    else
    {
        motor[4].direction = 0;
    }

    motor[6].pulse_num = (int)(first_arm_degree * degree_switch + 0.5f);
    // motor[6].speed = 150;

    motor[4].pulse_num = (int)(second_arm_degree * degree_switch + 0.5f);
    // motor[4].speed = 150;

    gimbal_motor_control_position(motor[4]);
    gimbal_motor_control_position(motor[6]);
}

/**
*@brief yz平面机械臂运动学逆解算
*@param y: y轴运动距离
*@param z: z轴运动距离
*@param out_arm_degree[0]: 大臂角度,竖直方向为0,向前为正角度
*@param out_arm_degree[1]: 小臂角度,水平方向为0,向上为正角度
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

    first_arm_degree = 90.0f - first_arm_degree;
    second_arm_degree = -90.0f + second_arm_degree - first_arm_degree;

    out_arm_degree[0] = first_arm_degree;   //大臂角度
    out_arm_degree[1] = second_arm_degree;  //小臂角度
}

/**
*@brief 机械臂运动学正解算
*@param first_arm_degree: 大臂角度,竖直方向为0,向前为正角度
*@param second_arm_degree: 小臂角度,水平方向为0,向上为正角度
*@param out_arm_location_yz[0]: 输出坐标y
*@param out_arm_location_yz[1]: 输出坐标z
*@return void
*/
float* robot_arm_calculate_forward(float first_arm_degree, float second_arm_degree, float* out_arm_location_yz)
{
    float z1, z2, y1, y2;                                   //大臂和小臂的z轴和y轴增量

    z1 = first_arm_lenght * arm_cos_f32(first_arm_degree * angle_2_rad);
    y1 = first_arm_lenght * arm_sin_f32(first_arm_degree * angle_2_rad);
                                                                                    
    z2 = second_arm_lenght * arm_sin_f32(second_arm_degree * angle_2_rad);
    y2 = second_arm_lenght * arm_cos_f32(second_arm_degree * angle_2_rad);

    out_arm_location_yz[0] = y1 + y2 + gripper_lenght;
    out_arm_location_yz[1] = z1 + z2 - gripper_height + base_height;

    return out_arm_location_yz;
}

/**
*@brief 机械臂yz平面位置控制
*@param y: y坐标,单位mm
*@param z: z轴坐标,单位mm
*@return void
*/
void robot_arm_control_yz(float y, float z)
{
    float yz_to_angle[2];
    
    robot_arm_calculate_inverse(y, z, yz_to_angle);
    robot_arm_control_arm_poosition(yz_to_angle[0], yz_to_angle[1]);
}

/**
*@brief 获取电机当前位置
*@return void
*/
void robot_arm_get_position(void)
{
    get_motor_angle(motor[4]);
    get_motor_angle(motor[5]);
    get_motor_angle(motor[6]);

    delay_ms(10);
}

/**
*@brief 等待所有电机运动完毕
*@return void
*/
void robot_arm_wait_stop(void)
{
    get_motor_state(motor[6]);
    delay_ms(10);
}

/**
*@brief 串口3串口中断
*@param 无
*@return 无
*/
void USART3_IRQHandler(void)
{
    uint32_t data_buff;

    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)//空闲中断
    {
		USART3->DR; //清USART_IT_IDLE标志	不进行此步骤会一直进中断

		DMA_Cmd(DMA1_Stream1, DISABLE);
		DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1);

        //读取电机角度数据
        if(robot_arm_motor_rx_data[1] == 0x36)
        {
            float yz[2];
            float out_degree[2];

            data_buff = robot_arm_motor_rx_data[3] << 24 |
                        robot_arm_motor_rx_data[4] << 16 |
                        robot_arm_motor_rx_data[5] << 8  |
                        robot_arm_motor_rx_data[6];

            if(robot_arm_motor_rx_data[0] == 0x01) 
            {
                gimbal_motor_1_position = (float)data_buff * 360.0f / 65536.0f / reduction_ratio;
                if(robot_arm_motor_rx_data[2]) gimbal_motor_1_position = -gimbal_motor_1_position;
            }
            else if(robot_arm_motor_rx_data[0] == 0x02) 
            {
                gimbal_motor_2_position = (float)data_buff * 360.0f / 65536.0f / reduction_ratio;
                if(robot_arm_motor_rx_data[2]) gimbal_motor_2_position = -gimbal_motor_2_position;
            }
            else if(robot_arm_motor_rx_data[0] == 0x03) 
            {
                gimbal_motor_3_position = (float)data_buff * 360.0f / 65536.0f / reduction_ratio;
                if(robot_arm_motor_rx_data[2]) gimbal_motor_3_position = -gimbal_motor_3_position;
            }

            robot_arm_calculate_forward(gimbal_motor_3_position, gimbal_motor_1_position, yz);
            robot_arm_calculate_inverse(yz[0], yz[1], out_degree);

            printf("angle:%f %f check angle:%f %f location:%f %f botton angle:%f\r\n", 
                    gimbal_motor_3_position, gimbal_motor_1_position, 
                    out_degree[0] ,  out_degree[1],
                    yz[0], yz[1], gimbal_motor_2_position);
        }
        
        // printf("%s", robot_arm_motor_rx_data);

        //清除缓冲区
        for(char i = 0; i < 30; i++) robot_arm_motor_rx_data[i] = 0;

		USART_ClearFlag(USART3, USART_FLAG_IDLE);
		DMA_SetCurrDataCounter(DMA1_Stream1, 20);//设置传输数据长度
		DMA_Cmd(DMA1_Stream1, ENABLE);//打开DMA
    }
}