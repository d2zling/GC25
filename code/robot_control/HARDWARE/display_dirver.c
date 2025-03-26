#include "display_driver.h"

void display_init()
{
    gpio_init();

    MR(1);

    delay_ms(1000);

    DS(0);
    SHCP(0);
    STCP(0);
}

void _74hc595_write_bit(uint8_t bit)
{
    DS(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);

    delay_us(1);
    SHCP(0);
    delay_us(1);
    SHCP(1);



    
    STCP(0);
    delay_us(1);
    STCP(1);
    delay_us(1);
}

void _74hc595_write_byte(uint8_t _byte)
{
    for(char i = 0; i < 8; i++)
        _74hc595_write_bit((_byte >> (7 - i)) & 0x01);

    STCP(0);
    delay_us(1);
    STCP(1);
    delay_us(1);
}


void disp_num()
{
    _74hc595_write_byte(0xaa);
    _74hc595_write_byte(0xaa);
}