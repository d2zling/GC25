#include "ws2812.h"

void WS2812_Init(void)
{
    GPIO_InitTypeDef GPIO_cfg;
    GPIO_cfg.Mode = GPIO_OUT_PP;
    GPIO_cfg.Pin = GPIO_Pin_5;

    GPIO_Inilize(GPIO_P5, &GPIO_cfg);

    P55 = 0;
}

void WS2812_write_0(void)
{
    P55 = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    P55 = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void WS2812_write_1(void)
{
    P55 = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    P55 = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();

}

void WS2812_SendByte_all(char R, char G, char B)
{
    char i, j;

    for(j = 0; j < 9; j++)
    {
        for(i = 0; i < 8; i++)//G
        {
            if(G << i & 0x80) WS2812_write_1();
            else WS2812_write_0();
        }
        for(i = 0; i < 8; i++)//R
        {
            if(R << i & 0x80) WS2812_write_1();
            else WS2812_write_0();
        }
        for(i = 0; i < 8; i++)//B
        {
            if(B << i & 0x80) WS2812_write_1();
            else WS2812_write_0();
        }
    }
}

void WS2812_SendByte_single(char R, char G, char B)
{
	char i;

	for(i = 0; i < 8; i++)//G
	{
		if(G << i & 0x80) WS2812_write_1();
		else WS2812_write_0();
	}
	for(i = 0; i < 8; i++)//R
	{
		if(R << i & 0x80) WS2812_write_1();
		else WS2812_write_0();
	}
	for(i = 0; i < 8; i++)//B
	{
		if(B << i & 0x80) WS2812_write_1();
		else WS2812_write_0();
	}
}

//RGB数据转化未HSV数据
float* RGB2HSV(int R, int G, int B)
{
	static float rgb[3];

	float rgb_r = (float)R / 255;
	float rgb_g = (float)G / 255;
	float rgb_b = (float)B / 255;

	float V, S, H, min_rgb = 0;

	//寻最大数
	V = rgb_r > rgb_g ? rgb_r : rgb_g;
	V = rgb_b > V ? rgb_b : V;

	if (V != 0)
	{
		//寻最小数
		min_rgb = rgb_r < rgb_g ? rgb_r : rgb_g;
		min_rgb = rgb_b < min_rgb ? rgb_b : min_rgb;
		
		S = (V - min_rgb) / V;
	}
	else S = 0;

	if (V == rgb_r) H = 60 * (rgb_g - rgb_b) / (V - min_rgb);
	else if (V == rgb_g) H = 120 + 60 * (rgb_b - rgb_r) / (V - min_rgb);
	else if (V == rgb_b) H = 240 + 60 * (rgb_r - rgb_g) / (V - min_rgb);

	if (H < 0) H = H + 360;
	
	H = H / 2;
	S = S * 255;
	V = V * 255;

	rgb[0] = H;
	rgb[1] = S;
	rgb[2] = V;

	return rgb;
}

//HSV数据转化为RGB数据
float* HSV2RGB(float H, float S, float V)
{
	//printf("%f %f %f\r\n", H, S, V);
	static float RGB[3] = {0};//R,G,B格式
	float f, p, q, t;
	int h;

	H = H * 2;
	S = (S / 255);
	V = (V / 255);

	h = H / 60;
	f = H / 60 - h;

	p = V * (1 - S);
	q = V * (1 - f * S);
	t = V * (1 - (1 - f) * S);

	switch (h)
	{
	case 0:	RGB[0] = V;
			RGB[1] = t;
			RGB[2] = p; 
			break;

	case 1:	RGB[0] = q;
			RGB[1] = V;
			RGB[2] = p;
			break;

	case 2:	RGB[0] = p;
			RGB[1] = V;
			RGB[2] = t;
			break;

	case 3:	RGB[0] = p;
			RGB[1] = q;
			RGB[2] = V;
			break;

	case 4:	RGB[0] = t;
			RGB[1] = p;
			RGB[2] = V;
			break;

	case 5:	RGB[0] = V;
			RGB[1] = p;
			RGB[2] = q;
			break;

	default:
		break;
	}

	RGB[0] = RGB[0] * 255;
	RGB[1] = RGB[1] * 255;
	RGB[2] = RGB[2] * 255;

	return RGB;
}

void loop_show(void)
{
	char i, j, k;

    for(k = 0; k < 5; k++)		//一次
	for(j = 0; j < 9; j++)		//一圈
	{
		for(i = 0; i < 9; i++)	//一帧
		{
			if(j == i) WS2812_SendByte_single(0, 0xAA, 0xFF);
			else WS2812_SendByte_single(0, 0, 0);
		}

		delay_ms(50);
	}
}

void slowly_brighten_show(u8 brightness)
{
	u8 i, j;

	for(i = 0; i < 254; i++)
	{
		WS2812_SendByte_all(0,i,i);
		delay_ms(3);
	}

	j = 0;

	while(1)
	{
		if(i > brightness) i--;
		else if(i < brightness) i++;

		if(j < brightness) j++;

		WS2812_SendByte_all(j,i,i);
		delay_ms(5);

		if((i == brightness) && (j == brightness))  break;
	}

}	