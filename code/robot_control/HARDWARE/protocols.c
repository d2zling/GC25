#include "protocols.h"

/*
    此为通信协议文件，遥控器使用USART1与上位机通信
*/

//数据包对象head和tail为链表头尾，不可删除
static _usart_pack head;
static _usart_pack tail;	
static _usart_pack remote_control_data;
static _usart_pack pi_angel_data;
static _usart_pack pi_position_data;
static _usart_pack pi_location_data;
static _usart_pack pi_color_area_data;

//数据包
// static u8 remote_control_package[20] = {0};
static u8 pi_angel[20] = {0};
static u8 pi_position[20] = {0};
static u8 pi_location[20] = {0};
static u8 pi_color_area[20] = {0};

//DMA接收缓冲区
static u8 rec_usart1_buff[dma_buffer_size];
static u8 rec_usart2_buff[dma_buffer_size];

/**
*@brief 上下位机协议初始化
*@param 无
*@return 无
*/
void protocols_init(void)
{
	usart1_init(115200);	//树莓派通讯
	usart2_init(250000);	//调试通讯
	usart1_rx_dma_init(rec_usart1_buff, dma_buffer_size);
    usart2_rx_dma_init(rec_usart2_buff, dma_buffer_size);
	soft_packet_init();
}

/**
*@brief 串口包扫描
*@param rec_byte 串口中断接收到的数据
*@return 目前数据包的接收状态
*/
char packet_scan(u8 rec_byte)
{
	static _usart_pack* p_packger;
	static u8 p_rec_byte = 0, rec_flag = 0;
	
	if (rec_flag == 0)//接收到数据包头
	{
		p_packger = head.next_usart_pack;

		//搜寻包头
		while (p_packger->package_header != rec_byte)
		{
			p_packger = p_packger->next_usart_pack;
			if (p_packger == &tail)	return rec_err;
		}

		//清除上一次数据包
		memset(p_packger->packet, 0, p_packger->package_len);
		p_packger->package_flag = rec_unfinish;
		rec_flag = 1;
		return rec_header;
	}
	else if (p_packger->package_tail == rec_byte)//接收到数据包尾
	{
		rec_flag = p_rec_byte = 0;
		p_packger->package_flag = rec_finish;
		return rec_finish;
	}
	else if(rec_flag == 1)//接收到数据包数据
	{
		p_packger->packet[p_rec_byte] = rec_byte;
		p_rec_byte++;
		return rec_data;
	}

	return rec_err;
}

/**
*@brief 创建串口包
*@param packger 要创建的串口包对象
*@return 无
*/
void create_usart_package(_usart_pack* packger)
{
	_usart_pack* p_packger;

	if (head.next_usart_pack == &tail)
	{
		head.next_usart_pack = packger;
		packger->next_usart_pack = &tail;
	}
	else
	{
		for (p_packger = &head; p_packger->next_usart_pack != &tail; 
			p_packger = p_packger->next_usart_pack);

		p_packger->next_usart_pack = packger;
		packger->next_usart_pack = &tail;
	}
}

/**
*@brief 初始化软件数据包
*@param 无
*@return 无
*/
void soft_packet_init(void)
{
	head.next_usart_pack = &tail;
	tail.next_usart_pack = &head;

	pi_location_data.package_header = 'L';
	pi_location_data.package_tail = 'E';
	pi_location_data.packet = pi_location;
	pi_location_data.package_len = 20;
	pi_location_data.package_flag = rec_unfinish;
	create_usart_package(&pi_location_data);

	pi_color_area_data.package_header = 'C';
	pi_color_area_data.package_tail = 'E';
	pi_color_area_data.packet = pi_color_area;
	pi_color_area_data.package_len = 15;
	pi_color_area_data.package_flag = rec_unfinish;
	create_usart_package(&pi_color_area_data);
}

/**
*@brief 传入含数字类型的char数组，解算为float型数据
*@param packet 字符串中数字的首地址，如果没有'-'号则输入以空格为首的字符串，有则以'-'号为首
*@return 解算后的float数据
*/
float decode_remote_control_num(u8* packet)
{
	float num = 0.0;
	num = packet[1] - '0';
	num += ((packet[3] - '0' ) * 0.1);
	num += ((packet[4] - '0' ) * 0.01);
	num += ((packet[5] - '0' ) * 0.001);
	if(packet[0] == '-') num = -num;
	return num;
}

/**
*@brief 获取测试数据包，去除数据头与数据尾
		数据包样例 C['-0.008', '-0.047', ' 0.056', ' 0.016', '-1.000', '-1.000']E
*@param 无
*@return float类型的手柄数据数组，从0开始分别是x1, y1, x2, y2, l1, l2
*/
float* get_remote_control_data(void)
{
	u8* p_num = NULL;
	static float remote_control_data_float[6] = {0};

	if(remote_control_data.package_flag == rec_finish)
	{
		p_num = remote_control_data.packet + 2;
		for(char i = 0; i < 6; i++)
		{
			remote_control_data_float[i] = decode_remote_control_num(p_num);
			p_num = p_num + 10;
		}

		remote_control_data.package_flag = rec_unfinish;
		return remote_control_data_float;
	}
	else return NULL;
}

/**
*@brief 获取获取视觉角度数据
*@param 无
*@return int 10倍角度数据
*/
int get_pi_degree_data(void)
{
	int pi_degree_out = 0;
	if(pi_angel_data.package_flag == rec_finish)
	{
		pi_angel_data.package_flag = rec_unfinish;

		printf("%s ", pi_angel);

		for(char i = 0; i < 4; i++)
		{
			pi_angel[i] = pi_angel[i] - '0';
		}

		pi_angel[1] = pi_angel[1] * 100;
		pi_angel[2] = pi_angel[2] * 10;

		pi_degree_out = pi_angel[1] + pi_angel[2] + pi_angel[3];

		if(!pi_angel[0]) pi_degree_out = pi_degree_out * -1;

		return pi_degree_out;
	}
	else return NULL;
}

/**
*@brief 获取视觉坐标数据
*@param 无
*@return int* 获取坐标数组
*/
int* get_pi_position_data(void)
{
	static int pi_position_out[2];

	if(pi_position_data.package_flag == rec_finish)
	{
		pi_position_data.package_flag = rec_unfinish;
		printf("%s ", pi_position);

		for(char i = 0; i < 6; i++)
		{
			pi_position[i] = pi_position[i] - '0';
		}

		pi_position_out[0] = pi_position[0] * 100 +
							 pi_position[1] * 10  +
							 pi_position[2];

		pi_position_out[1] = pi_position[3] * 100 +
							 pi_position[4] * 10  +
							 pi_position[5];

		return pi_position_out;
	}
	else return NULL;
}

/**
*@brief 获取视觉 角度 坐标 数据
*@param 无
*@return int* 获取 角度 坐标 数组
*/
int* get_pi_location_data(void)
{
	static int location_data[3];

	if(pi_location_data.package_flag == rec_finish)
	{
		pi_location_data.package_flag = rec_unfinish;
		//printf("%s ", pi_location);

		for(char i = 0; i < 10; i++)
		{
			pi_location[i] = pi_location[i] - '0';
		}

		location_data[0] = 	pi_location[1] * 100 +
							pi_location[2] * 10  + 
							pi_location[3];

		location_data[1] = 	pi_location[4] * 100 +
							pi_location[5] * 10  + 
							pi_location[6];

		location_data[2] = 	pi_location[7] * 100 +
							pi_location[8] * 10  + 
							pi_location[9];

		if(!pi_location[0]) location_data[0] = location_data[0] * -1;

		return location_data;
	}
	else return NULL;
}

/**
*@brief 获取视觉 颜色 位置 数据
*@param 无
*@return char*  颜色 位置 数组
*/
u8* get_pi_color_area_data(void)
{
	if(pi_color_area_data.package_flag == rec_finish)
	{
		pi_color_area_data.package_flag = rec_unfinish;

		return pi_color_area;
	}
	else return NULL;
}

/**
*@brief 获取扫码模块数据包
*@param 无
*@return char* 数据包首地址
*/
char* get_scanner_data(void)
{
	if(qr_data_packge[9] == rec_finish) 
		return qr_data_packge;
	else 
	{
		qr_data_packge[9] = rec_unfinish;
		return NULL;
	}
}

/**
*@brief 发送数据包给树莓派
*@param call_pi_data 要发送的数据
*@return void
*/
void call_pi(char *call_pi_data)
{
	for(char i = 0; i < 3; i++)
	{
		while((USART1->SR&0X40)==0);
		USART1->DR = call_pi_data[i];
	}
}

/**
*@brief 串口1中断服务程序，收发视觉数据
*@param 无
*@return 无
*/
void USART1_IRQHandler(void)            
{
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->DR;

        DMA_Cmd(DMA2_Stream2, DISABLE);
		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		
		printf(" %s\r\n",rec_usart1_buff);
		for(char i = 0; packet_scan(rec_usart1_buff[i]); i++);

        USART_ClearFlag(USART1, USART_FLAG_IDLE);
		DMA_SetCurrDataCounter(DMA2_Stream2, dma_buffer_size);//设置传输数据长度
		DMA_Cmd(DMA2_Stream2, ENABLE);//打开DMA
    }
}

/**
*@brief 串口2中断服务程序,接收调试器数据
*@param 无
*@return 无
*/
void USART2_IRQHandler(void)            
{
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        USART2->DR;

        DMA_Cmd(DMA1_Stream5, DISABLE);
		DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF2);

		for(char i = 0; packet_scan(rec_usart2_buff[i]); i++);

        USART_ClearFlag(USART2, USART_FLAG_IDLE);
		DMA_SetCurrDataCounter(DMA1_Stream5, dma_buffer_size);//设置传输数据长度
		DMA_Cmd(DMA1_Stream5, ENABLE);//打开DMA
    }
}

/**
*@brief 串口5中断函数，实现扫码接收功能
*@param 无
*@return 无
*/
void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		u8 scan_data = USART_ReceiveData(UART5);
		rec_scanner_data(scan_data);
		//printf("%c", scan_data);
	}
}