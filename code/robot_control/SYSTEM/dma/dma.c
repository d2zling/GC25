#include "dma.h"

/**
*@brief 串口1接收DMA初始化
*@param dma_buf 接收缓冲区地址
*@param buff_size 接收缓冲区大小
*@return 无
*/
void usart1_rx_dma_init(u8* dma_buf, uint32_t buff_size)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2时钟使能 

    DMA_DeInit(DMA2_Stream2);
	while (DMA_GetCmdStatus(DMA2_Stream2) != DISABLE){}//等待DMA可配置 

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;//通道选择 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//DMA 外设地址 
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dma_buf;//DMA 存储器 0 地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = buff_size;//数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设非增量模式 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度:8位 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级 
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//单次传输 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输 
	DMA_Init(DMA2_Stream2, &DMA_InitStructure);//初始化DMA Stream

	DMA_Cmd(DMA2_Stream2, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);//使能串口DMA接收数据
}

/**
*@brief 串口2接收DMA初始化
*@param dma_buf 接收缓冲区地址
*@param buff_size 接收缓冲区大小
*@return 无
*/
void usart2_rx_dma_init(u8* dma_buf, uint32_t buff_size)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//DMA1时钟使能 

    DMA_DeInit(DMA1_Stream5);
	while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE){}//等待DMA可配置 

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;//通道选择 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;//DMA 外设地址 
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dma_buf;//DMA 存储器 0 地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = buff_size;//数据传输量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设非增量模式 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度:8位 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级 
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//单次传输 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输 
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);//初始化DMA Stream

	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);	//清除发送完成标识
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);	//使能串口DMA接收数据
	DMA_Cmd(DMA1_Stream5, ENABLE);
}

/**
*@brief 串口6接收DMA初始化
*@param dma_buf 接收缓冲区地址
*@param buff_size 接收缓冲区大小
*@return 无
*/
void usart6_rx_dma_init(u8* dma_buf, uint32_t buff_size)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2时钟使能 

    DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA可配置 

	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;//通道选择 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;//DMA 外设地址 
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dma_buf;//DMA 存储器 0 地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式 
	DMA_InitStructure.DMA_BufferSize = buff_size;//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设非增量模式 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度:8位 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据长度:8位 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式  
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级 
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;          
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//单次传输 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输 
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);//初始化DMA Stream

	DMA_Cmd(DMA2_Stream1, ENABLE);
	USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);//使能串口DMA接收数据
}

