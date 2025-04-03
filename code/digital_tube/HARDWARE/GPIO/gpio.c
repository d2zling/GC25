#include "gpio.h"

//初始化TB6612输入引脚
void _3_8_PIN_LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    GPIO_InitStructure.GPIO_Pin = A | B | C | D | E | F | G | DP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz
    GPIO_Init(_8_LED_GPIO, &GPIO_InitStructure);		

    GPIO_InitStructure.GPIO_Pin = A_DIG1 | A_DIG2 | A_DIG3 | B_DIG1 | B_DIG2 | B_DIG3;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz
    GPIO_Init(_3_LED_GPIO, &GPIO_InitStructure);	

    GPIO_ResetBits(_8_LED_GPIO, all_reset);

    GPIOB->ODR = 0x00;
}

void num_show(char num)
{
    GPIO_ResetBits(_8_LED_GPIO, all_reset);

    switch (num)
    {
        case '0':   GPIO_SetBits(_8_LED_GPIO, all_set); break;
        case '1':   GPIO_SetBits(_8_LED_GPIO, show_1);  break;
        case '2':   GPIO_SetBits(_8_LED_GPIO, show_2);  break;
        case '3':   GPIO_SetBits(_8_LED_GPIO, show_3);  break;
        
        default:    GPIO_SetBits(_8_LED_GPIO, show_ERR);
    }
}

//显示一次6.15ms
void LED_display(char *display_data, int delay_time)
{
    GPIOB->ODR = A_DIG1;
    num_show(display_data[0]);
		delay_us(50);
    GPIOB->ODR = A_DIG2;
    num_show(display_data[1]);
		delay_us(50);
    GPIOB->ODR = A_DIG3;
    num_show(display_data[2]);
		delay_us(50);

    GPIOB->ODR = B_DIG1;
    num_show(display_data[3]);
		delay_us(50);
    GPIOB->ODR = B_DIG2;
    num_show(display_data[4]);
		delay_us(50);
    GPIOB->ODR = B_DIG3;
    num_show(display_data[5]);
		delay_us(50);
	
		GPIO_SetBits(_8_LED_GPIO, all_reset);
		delay_us(delay_time);
}

