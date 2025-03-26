#include "REG52.H"

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;

typedef signed char int8_t;
typedef signed int  int16_t;
typedef signed long int32_t;

#define EXT0_VECTOR     0  /* 0x03 external interrupt 0 */
#define TIM0_VECTOR     1  /* 0x0b timer 0 */
#define EXT1_VECTOR     2  /* 0x13 external interrupt 1 */
#define TIM1_VECTOR     3  /* 0x1b timer 1 */
#define UART0_VECTOR    4  /* 0x23 serial port 0 */

//´æ´¢0-FÂë±íÊý×é
const volatile uint8_t led_disp_data[] = {0x3F, 0x06, 0x5B, 0x4F, 
                                          0x66, 0x6D, 0x7D, 0x07, 
                                          0x7F, 0x6F, 0x77, 0x7C, 
                                          0x39, 0x5E, 0x79, 0x71};   

sbit SH_CP  = P0^0;
sbit DS     = P0^1;
sbit ST_CP  = P0^2;
sbit MR     = P0^3;

void _74hc595_write_bit(uint8_t _bit)
{
    SH_CP = 0;
    DS = _bit;
    SH_CP = 1;
}

void _74hc595_write_byte(uint8_t _byte)
{
    char i;

    for(i = 0; i < 8; i++)
    _74hc595_write_bit((_byte >> (7 - i)) & 0x01);

    ST_CP = 0;
    ST_CP = 1;
}

void main()
{
    _74hc595_write_byte(led_disp_data[0]);
    _74hc595_write_byte(led_disp_data[1]);
    _74hc595_write_byte(led_disp_data[2]);
    _74hc595_write_byte(led_disp_data[3]);
    _74hc595_write_byte(led_disp_data[4]);
    _74hc595_write_byte(led_disp_data[10]);

    while (1)
    {
        // TODO
    }
}

