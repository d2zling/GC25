
/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了宏晶科技的资料及程序   */
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

本文件为STC15xxx系列的延时程序,用户几乎可以不修改这个程序.

******************************************/

#include "delay.h"

//========================================================================
// 函数: void  delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注:
//========================================================================
void delay_ms(unsigned char ms)
{
	unsigned int i;
	do
	{
		i = MAIN_Fosc / 13000;
		while (--i)
			; //14T per loop
	} while (--ms);
}

//-----------------------------------

#define T0_IsTimeout() TF0 == 1
#define T0_ResetTimeout() TF0 = 0

u8 _unit;
u16 _loopMax, _loopCount, _lastReloadVal;

void DelayInit()
{
	TIM_InitTypeDef timerDef;
	timerDef.TIM_Mode = TIM_16BitAutoReload;
	timerDef.TIM_ClkOut = DISABLE;
	timerDef.TIM_ClkSource = TIM_CLOCK_1T;
	timerDef.TIM_Interrupt = DISABLE;
	timerDef.TIM_Polity = PolityLow;
	timerDef.TIM_Run = DISABLE;
	timerDef.TIM_Value = 0x00;

	_unit = MainFosc_KHZ / 1000U;

	Timer_Inilize(0, &timerDef);
}

void Delayms(u16 ms)
{
	unsigned long val = ms * _unit * 1000UL;
	_loopMax = val / 0x10000U + 1;
	_lastReloadVal = 0x10000U - (val % 0x10000U);
	_loopCount = 0;

	TR0 = 0;

	Timer0_Load(_loopMax == 1 ? _lastReloadVal : 0x00);

	TR0 = 1;

	while (_loopCount < _loopMax)
	{
		while (!T0_IsTimeout())
			;

		T0_ResetTimeout();

		_loopCount++;

		if (_loopCount == _loopMax - 1)
		{
			Timer0_Load(_lastReloadVal);
		}
	}

	TR0 = 0;
}