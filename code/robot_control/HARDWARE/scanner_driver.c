#include "scanner_driver.h"

/*
    此为扫码模块驱动文件，用于实现扫码模块的初始化、数据接收等功能。
*/

char qr_data_packge[10];

/**
*@brief 初始化扫码模块
*@param 无
*@return 无
*/
void scanner_init(void)
{
    uart5_init(9600);
	qr_data_packge[9] = rec_unfinish;
}


/**
*@brief 扫码数据接收实现
*@param rec_qr_data: 接收到的二维码数据
*@return 无
*/
u8 rec_scanner_data(u8 rec_qr_data)
{
    static char p_scanner_data = 0; 

    qr_data_packge[p_scanner_data] = rec_qr_data;
    p_scanner_data++;
    
   //扫码完成
    if(p_scanner_data == 7)
    {
        qr_data_packge[9] = rec_finish;
        p_scanner_data = 0;
        USART_DeInit(UART5);
    }
    
    return rec_unfinish;
}