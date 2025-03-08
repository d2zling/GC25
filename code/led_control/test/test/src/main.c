#include <intrins.h>
// #include <stdio.h>
// stc15w204s是最小化的芯片只有8脚,是一个缩水版本,注意虽然本程序是跑在stc15w204s上的但是头文件依然用的是这个"完整版的"15系类头文件
#include <STC15F2K60S2.H>
// 定义常用的语句
typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;
//串口发送忙标志位
bit busy = 0;
//当前串口速率和内部RC晶振频率是9600bps@11.0592MHz,1T
//这个芯片没有定时器1,只有0和2
void UartInit(void) {
	//8位数据,可变波特率
	SCON = 0x50;
	//串口1选择定时器2为波特率发生器
	AUXR |= 0x01;
	//定时器2时钟为Fosc,即1T
	AUXR |= 0x04;
	//内部RC时钟设置为11.0592的定时器2初值设置
	//设定定时初值
	T2L = 0xE0;
	//设定定时初值
	T2H = 0xFE;
	//启动定时器2
	AUXR |= 0x10;
	//允许UART串口中断
	ES = 1 ;
	//允许总中断
	EA = 1 ;
}
//软件延时,肯定不止1s,演示用
void delay1s() {
	unsigned char i, j, k;
	_nop_();
	_nop_();
	i = 43;
	j = 6;
	k = 203;
	do {
		do {
			while (--k);
		}
		while (--j);
	}
	while (--i);
}
// 中断入口
void Uart() interrupt 4 {
	// 接收完毕导致的中断
	if(RI) {
		// 判断是复位到ISP的串口指令
		if(SBUF == 0xAA) {
			//立即关闭串口中断,防止延迟期再次进入中断
			ES = 0;
			//延迟一秒后重启单片机到isp区
			delay1s();
			//复位到ISP
			IAP_CONTR = 0x60;
			//while(1);
			return;
		}
		RI = 0;
		SBUF = SBUF;
	}
	// 发送完毕中断
	if (TI) {
		// 清零发送中断标志位
		TI = 0;
		// 
		busy = 0;
	}
}
//打印字符串
void printString(char *p) {
	while(*p) {
		busy = 1;
		SBUF = *p++;
		while(busy);
	}
}
//主程序入口
void main(void) {
	// 计数器
	u8 tmp = 0x00;
	// 定义字符串
	char *s = "CSDN!\r\n";
	UartInit();
	while(1) {
		//观察LED电平翻转
		P33=!P33;
		printString(s);
		delay1s();
		tmp++;
	}
}