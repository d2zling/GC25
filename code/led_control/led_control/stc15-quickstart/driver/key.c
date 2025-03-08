#include "key.h"

char key_flag_0 = 0;
char key_flag_1 = 0;

void key_Init(void)
{
    GPIO_InitTypeDef GPIO_cfg;
    // P32 = P33 = 1;

    // EA = 1;
    // IT0 = 0;
    // IT1 = 0;
    // EX0 = 1;
    // EX1 = 1;

    GPIO_cfg.Mode = GPIO_PullUp;
    GPIO_cfg.Pin = GPIO_Pin_2 | GPIO_Pin_3;

    GPIO_Inilize(GPIO_P3, &GPIO_cfg);
}

void key_scan_0(void) interrupt 0
{
    if(key_flag_0 == 0) 
    {
        key_flag_0 = 1;
        TxSend('1');
    }
    else 
    {
        key_flag_0 = 0;
        TxSend('0');
    }
    delay_ms(500);
}

void key_scan_1(void) interrupt 2
{
    if(key_flag_1 == 0) 
    {
        key_flag_1 = 1;
        TxSend('1');
    }
    else 
    {
        key_flag_1 = 0;
        TxSend('0');
    }
    delay_ms(500);
}