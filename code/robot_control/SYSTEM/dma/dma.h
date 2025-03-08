#ifndef __DMA_H_
#define __DMA_H_

#include "stm32f4xx.h"

void usart1_rx_dma_init(u8* dma_buf, uint32_t buff_size);
void usart2_rx_dma_init(u8* dma_buf, uint32_t buff_size);
void usart3_rx_dma_init(u8* dma_buf, uint32_t buff_size);
void usart6_rx_dma_init(u8* dma_buf, uint32_t buff_size);

#endif