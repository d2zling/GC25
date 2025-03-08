#include <intrins.h>
// #include <stdio.h>
// stc15w204s����С����оƬֻ��8��,��һ����ˮ�汾,ע����Ȼ������������stc15w204s�ϵĵ���ͷ�ļ���Ȼ�õ������"�������"15ϵ��ͷ�ļ�
#include <STC15F2K60S2.H>
// ���峣�õ����
typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;
//���ڷ���æ��־λ
bit busy = 0;
//��ǰ�������ʺ��ڲ�RC����Ƶ����9600bps@11.0592MHz,1T
//���оƬû�ж�ʱ��1,ֻ��0��2
void UartInit(void) {
	//8λ����,�ɱ䲨����
	SCON = 0x50;
	//����1ѡ��ʱ��2Ϊ�����ʷ�����
	AUXR |= 0x01;
	//��ʱ��2ʱ��ΪFosc,��1T
	AUXR |= 0x04;
	//�ڲ�RCʱ������Ϊ11.0592�Ķ�ʱ��2��ֵ����
	//�趨��ʱ��ֵ
	T2L = 0xE0;
	//�趨��ʱ��ֵ
	T2H = 0xFE;
	//������ʱ��2
	AUXR |= 0x10;
	//����UART�����ж�
	ES = 1 ;
	//�������ж�
	EA = 1 ;
}
//�����ʱ,�϶���ֹ1s,��ʾ��
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
// �ж����
void Uart() interrupt 4 {
	// ������ϵ��µ��ж�
	if(RI) {
		// �ж��Ǹ�λ��ISP�Ĵ���ָ��
		if(SBUF == 0xAA) {
			//�����رմ����ж�,��ֹ�ӳ����ٴν����ж�
			ES = 0;
			//�ӳ�һ���������Ƭ����isp��
			delay1s();
			//��λ��ISP
			IAP_CONTR = 0x60;
			//while(1);
			return;
		}
		RI = 0;
		SBUF = SBUF;
	}
	// ��������ж�
	if (TI) {
		// ���㷢���жϱ�־λ
		TI = 0;
		// 
		busy = 0;
	}
}
//��ӡ�ַ���
void printString(char *p) {
	while(*p) {
		busy = 1;
		SBUF = *p++;
		while(busy);
	}
}
//���������
void main(void) {
	// ������
	u8 tmp = 0x00;
	// �����ַ���
	char *s = "CSDN!\r\n";
	UartInit();
	while(1) {
		//�۲�LED��ƽ��ת
		P33=!P33;
		printString(s);
		delay1s();
		tmp++;
	}
}