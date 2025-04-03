#include "delay.h"
#include "sys.h"
#include "gpio.h"
#include "power.h"
#include "usart.h"

//display_data定义在usart.c文件

int main(void)
{
	delay_init();	    	 //延时函数初始化	  
	uart1_Init(115200);
	_3_8_PIN_LED_Init();
	
  while(1) 
	{
			for(int a = 100; a < 1000; a++)
				LED_display(display_data, a);
			for(int a = 1000; a > 100; a--)
				LED_display(display_data, a);
	}
}

