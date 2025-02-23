#include "hwt101_driver.h"

/*
	此为HWT101驱动程序，使用USART6与HWT101通信，数据通过extern形式导入到其他文件中使用
*/

static u8 hwt101_data_buf[buff_size];

//陀螺仪输出角速度数据
#if __HWT101_ACC
	//hwt101角加速度
	float hwt101_acceleration_data = 0;	
#endif

//陀螺仪输出航向角数据
#if __HWT101_DEG
	//hwt101角度
	float hwt101_degree_data = 0.0f;
	//hwt101上次角度
	float last_hwt101_degree_data = 0.0f;
	//计算后的世界坐标角度
	float abs_degree_data = 0.0f;
	//旋转圈数计算
	int hwt101_circle_count = 0;
#endif

/**
*@brief hwt101陀螺仪初始化
*@param 无
*@return 无
*/
void hwt101_init()
{
	usart6_init(115200);
	usart6_rx_dma_init(hwt101_data_buf, buff_size);
}

/**
*@brief 串口6串口中断，主要用于接收陀螺仪数据
*@param 无
*@return 无
*/
void USART6_IRQHandler(void)
{
    if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)//空闲中断
    {
		USART6->DR; //清USART_IT_IDLE标志	不进行此步骤会一直进中断

		DMA_Cmd(DMA2_Stream1, DISABLE);
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);

#if __HWT101_ACC
		//校验加速度和解包
		if((u8)(0xa7 + hwt101_data_buf[6] + hwt101_data_buf[7]) == hwt101_data_buf[10])
		{
			hwt101_acceleration_data = ((float)((hwt101_data_buf[7] << 8) | hwt101_data_buf[6])) / 32768 * 2000;
		}
#endif

#if __HWT101_DEG
		//校验角度和解包
		if((u8)(0xa8 + hwt101_data_buf[17] + hwt101_data_buf[18] + hwt101_data_buf[19] + hwt101_data_buf[20]) == hwt101_data_buf[21])
		{
			last_hwt101_degree_data = hwt101_degree_data;//记录上一次角度
			hwt101_degree_data = ((float)((hwt101_data_buf[18] << 8) | hwt101_data_buf[17])) / 32768.0f * 180.0f;//得出角度数据

			if(last_hwt101_degree_data > 270.0f && hwt101_degree_data < 90.0f) hwt101_circle_count++;
			else if(last_hwt101_degree_data < 90.0f && hwt101_degree_data > 270.0f) hwt101_circle_count--;
			
			abs_degree_data = (float)hwt101_circle_count * 360.0f + hwt101_degree_data;
		}
#endif

		USART_ClearFlag(USART6, USART_FLAG_IDLE);
		DMA_SetCurrDataCounter(DMA2_Stream1, buff_size);//设置传输数据长度
		DMA_Cmd(DMA2_Stream1, ENABLE);//打开DMA
    }
}

/**
*@brief 获取零偏指令，产生零偏问题时使用
*@param 无
*@return 无
*/
void get_drift_mode(void)
{
	u8 get_drift_buff[] = {0xFF, 0xAA, 0x48, 0x01, 0x00};
	for(u8 i = 0; i < 5; i++)
	{
		while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
		USART6->DR = get_drift_buff[i];   
	}
}

/**
*@brief 恢复正常模式
*@param 无
*@return 无
*/
void normal_mode(void)
{
	u8 normal_buff[] = {0xFF, 0xAA, 0x48, 0x00, 0x00};
	for(u8 i = 0; i < 5; i++)
	{
		while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
		USART6->DR = normal_buff[i];   
	}
}

/**
*@brief 零漂角度校准
*@param 无
*@return 无
*/
void drift_calibration(void)
{
	get_drift_mode();
	for(u8 i=0;i<25;i++) delay_ms(1000);
	normal_mode();
}

/**
*@brief 设置输出速度100HZ
*@param 无
*@return 无
*/
void set_out_put_speed(void)
{
	u8 speed_buff[] = {0xFF, 0xAA, 0x03, 0x09, 0x00};
	for(u8 i = 0; i < 5; i++)
	{
		while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
		USART6->DR = speed_buff[i];   
	}
}

/**
*@brief 复位陀螺仪角度
*@param 无
*@return 无
*/
void reset_angle(void)
{
	u8 reset_buff[] = {0xFF, 0xAA, 0x76, 0x00, 0x00};
	for(u8 i = 0; i < 5; i++)
	{
		while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
		USART6->DR = reset_buff[i];   
	}
}