#ifndef __SCANNER_DRIVER_H
#define __SCANNER_DRIVER_H

#include "usart.h"

#define rec_finish      0
#define rec_unfinish    1

//扫码获取数据
extern char qr_data_packge[10];

void scanner_init(void);
u8 rec_scanner_data(u8 rec_qr_data);

#endif