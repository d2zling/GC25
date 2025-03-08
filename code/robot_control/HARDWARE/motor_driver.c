#include "motor_driver.h"

/*
    此为电机驱动文件，用于控制四个底盘电机和三个云台电机的速度和方向，使用UART4与电机进行通信。
*/

//四个电机结构体对象
//0~3为底盘电机
//4~6为云台电机
steeping_motor motor[7];

/**
*@brief 电机设置零点
*@param _motor 电机对象
*@return 无
*/
void motor_set_zero_point(steeping_motor _motor)
{
	char set_zero_point[13] = {	_motor.ID, 	
								0x93, 					
								0x88, 	
								0x01, 
								0x6b};		

	for(char i = 0; i < 5; i++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = set_zero_point[i];    
	}

	delay_ms(1);
}

/**
*@brief 电机回到零点
*@param _motor 电机对象
*@return 无
*/
void motor_turn_to_zero(steeping_motor _motor)
{
	char turn_to_zero[13] = {	_motor.ID, 	
								0x9A, 					
								0x00, 	
								0x00, 
								0x6b};		

	for(char i = 0; i < 5; i++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = turn_to_zero[i];    
	}

	delay_ms(1);
}

/**
*@brief 初始化电机控制串口外设
*@param 无
*@return 无
*/
void chassis_motor_init(void)
{
	uart4_init(115200);

    //软件初始化电机
    for(char i = 0; i < 4; i++) 
    {
        motor[i].ID = i + 1;
        motor[i].synchronous = 0;//不启用同步
        motor[i].acc_speed = 0x0;//不启用加速度
    }
}

/**
*@brief 单个底盘电机控制函数
*@param _motor 电机对象
*@return 无
*/
void chassis_motor_control_speed(steeping_motor _motor)
{
    char ctrl_speed_arr[8] = {	_motor.ID,
                                0xF6,
                                _motor.direction,
                                _motor.speed >> 8,
                                _motor.speed,
                                _motor.acc_speed,
                                _motor.synchronous,
                                0x6B};

    for(char i = 0; i < 8; i++)
    {
        while((UART4->SR&0X40)==0);
        UART4->DR = ctrl_speed_arr[i];
    }
    delay_ms(1);
}

/**
*@brief 底盘电机同步控制函数
*@param void
*@return 无
*/
void chassis_motor_control_synchronous(void)
{
	char synchronous_move[4] = {0x00, 0xff, 0x66, 0x6b};

	for(char i = 0; i < 4; i++)
	{
		while((UART4->SR&0X40)==0);
		UART4->DR = synchronous_move[i];
	}
	delay_ms(1);
}

/**
*@brief 初始化电机控制串口外设
*@param 无
*@return 无
*/
void gimbal_motor_init(void)
{
	usart3_init(115200);

	//软件初始化电机
    for(char i = 4; i < 7; i++) 
    {
        motor[i].ID = i - 3;
        motor[i].synchronous = 0;//不启用同步
        motor[i].acc_speed = 150;
		motor[i].speed = 150;
    }
}

/**
*@brief 单个云台电机控制速度函数
*@param _motor 电机对象
*@return 无
*/
void gimbal_motor_control_speed(steeping_motor _motor)
{
	char ctrl_speed_arr[8] = {	_motor.ID,
								0xF6,
								_motor.direction,
								_motor.speed >> 8,
								_motor.speed,
								_motor.acc_speed,
								_motor.synchronous,
								0x6B};

    for(char i = 0; i < 8; i++)
    {
        while((USART3->SR&0X40)==0);
        USART3->DR = ctrl_speed_arr[i];
    }
	
    delay_ms(1);
}

/**
*@brief 单个云台电机控制位置函数
*@param _motor 电机对象
*@return 无
*@note 	使用绝对位置控制
*/
void gimbal_motor_control_position(steeping_motor _motor)
{
	char ctrl_location_arr[13] = {	_motor.ID, 	
									0xfd, 					
									_motor.direction, 	
									_motor.speed >> 8, 
									_motor.speed, 		
									_motor.acc_speed,	
									_motor.pulse_num >> 24,
									_motor.pulse_num >> 16,
									_motor.pulse_num >> 8,
									_motor.pulse_num , 
									0x01,						//绝对位置控制
									_motor.synchronous,		
									0x6b};					

	for(char i = 0; i < 13; i++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = ctrl_location_arr[i];    
	}

	delay_ms(1);
}

/**
*@brief 单个云台电机获取旋转角度
*@param _motor 电机对象
*@return 无
*/
void get_motor_angle(steeping_motor _motor)
{
	char get_angle[3] = {	_motor.ID, 	
		 					0x36,
		 					0x6b};					

	for(char i = 0; i < 3; i++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = get_angle[i];    
	}

	delay_ms(1);
}

/**
*@brief 单个云台电机失能闭环控制
*@param _motor 电机对象
*@return 无
*/
void motor_pow_control(steeping_motor _motor, char state)
{
	char pow_ctrl[6] = {_motor.ID, 	
							0xf3,
							0xab,
							state,
							0x00,
							0x6b};					

	for(char i = 0; i < 6; i++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = pow_ctrl[i];    
	}

	delay_ms(1);
}