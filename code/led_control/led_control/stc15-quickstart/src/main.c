/**
 * 软件 UART：
 *      TX 引脚：P31
 *      波特率：9600
*/

#include "config.h"
#include "soft_uart.h"
#include "ws2812.h"
#include "key.h"
#include "EEPROM.h"

void main()
{
    unsigned int count_time = 0;
    u8 brightness, last_brightness;

    WS2812_Init();
    key_Init();

    delay_ms(1000);

    EEPROM_read_n(0x0000, &brightness, 1);
    TxSend(brightness);

    slowly_brighten_show(brightness);

    last_brightness = brightness;

    while (1)
    {
        //0按下
        if(P32 == 0) 
        {
            if(brightness == 0) brightness = 0;
            else  brightness -- ;
        }
        else if(P33 == 0) 
        {
            if(brightness == 0xFF) brightness = 0xFF;
            else brightness ++;
        }

        WS2812_SendByte_all(brightness, brightness, brightness);

        // TxSend(brightness);

        count_time++;

        if((count_time >= 300) && (last_brightness != brightness))
        {
            EEPROM_SectorErase(0x0000);
            EEPROM_write_n(0x0000, &brightness, 1);
            delay_ms(2000);
            count_time = 0;
            last_brightness = brightness;
        }

        delay_ms(5);
    }
}
