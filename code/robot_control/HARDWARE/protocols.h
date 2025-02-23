#ifndef __PROTOCOLS_H
#define __PROTOCOLS_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "usart.h"
#include "dma.h"
#include "scanner_driver.h"

//接收标志位
#define rec_finish      0
#define rec_unfinish    1
#define rec_header      2
#define rec_data        3
#define rec_err         4

#define dma_buffer_size 120

#define color       "@1#"
#define position    "@2#"
#define location    "@3#"

//数据包结构体
typedef struct usart_pack
{
/*--------初始化时需配置参数-----------*/
    u8* packet;         //包
    u8 package_header;  //包头
    u8 package_tail;    //包尾
    u16 package_len;    //包长度
    u8 package_flag;    //数据包标志位 rec_finish：接收完成  rec_unfinish：接收中
/*------------end---------------------*/

    struct usart_pack *next_usart_pack; //指向下一个数据包

}_usart_pack;

void protocols_init(void);
void soft_packet_init(void);

float* get_remote_control_data(void);

int get_pi_degree_data(void);
int* get_pi_position_data(void);
int* get_pi_location_data(void);
u8* get_pi_color_area_data(void);
void call_pi(char *call_pi_data);

char* get_scanner_data(void);

#endif