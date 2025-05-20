
#include "fnd.h"

void init_fnd(void);
int fnd_main(void);
void fnd_display(void);
void fnd_big_circle_forward(void);
void fnd_big_circle_backward(void);
void fnd_stop_display(void);

extern volatile int msec_count;
extern volatile int sec_count;
extern volatile int msec2_count;
extern volatile int sec2_count;
volatile int temp;

volatile int display_time = 0;

int fnd_main(void)
{
	while (1)
	{
		fnd_display();
		_delay_ms(1);
		fnd_big_circle_backward();
		_delay_ms(1);
	}
	
	return 0;
}

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;
	FND_DIGIT_DDR |= 1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4;
	FND_DIGIT_DDR |= 1 << New_FND_DIGIT_D1 | 1 << New_FND_DIGIT_D2 | 1 << New_FND_DIGIT_D3 | 1 << New_FND_DIGIT_D4;
	FND_DIGIT_PORT = ~0x00;
}

void fnd_display(void)
{
	uint8_t fnd_font[] = {0xc0, 0xf9, 0xA4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90, 0x7f};

	static int digit_select = 0;
	
	int value = display_time;
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT = 0x80;
		FND_DATA_PORT = fnd_font[value % 10];
		break;
		case 1:
		FND_DIGIT_PORT = 0x40;
		FND_DATA_PORT = fnd_font[(value / 10) % 10];
		break;
		case 2:
		FND_DIGIT_PORT = 0x20;
		FND_DATA_PORT = fnd_font[0];
		break;
		case 3:
		FND_DIGIT_PORT = 0x10;
		FND_DATA_PORT = fnd_font[0];
		break;
	}
	digit_select++;
	digit_select %= 4;
}


void fnd_big_circle_forward(void)
{
	uint8_t fnd_font0[] = {0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf6, 0xf4, 0xf0};
	uint8_t fnd_font1[] = {0xff, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf6, 0xf6, 0xf6, 0xf6};
	uint8_t fnd_font2[] = {0xff, 0xff, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6};
	uint8_t fnd_font3[] = {0xff, 0xff, 0xff, 0xf7, 0xe7, 0xc7, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6};

	static int digit_select = 0;
	// msec2_count는 밀리초 단위이므로, 50ms마다 프레임이 전환되도록 설정 (12 프레임 애니메이션)
	int frame = (msec2_count / 50) % 12;
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT = 0x08;
		FND_DATA_PORT = fnd_font0[frame];
		break;
		case 1:
		FND_DIGIT_PORT = 0x04;
		FND_DATA_PORT = fnd_font1[frame];
		break;
		case 2:
		FND_DIGIT_PORT = 0x02;
		FND_DATA_PORT = fnd_font2[frame];
		break;
		case 3:
		FND_DIGIT_PORT = 0x01;
		FND_DATA_PORT = fnd_font3[frame];
		break;
	}
	digit_select++;
	digit_select %= 4;
}

void fnd_big_circle_backward(void)
{
	uint8_t fnd_font0[] = {0xfb, 0xf9, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0};
	uint8_t fnd_font1[] = {0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xf6, 0xf6};
	uint8_t fnd_font2[] = {0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xf6, 0xf6, 0xf6};
	uint8_t fnd_font3[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xde, 0xce, 0xc6, 0xc6, 0xc6, 0xc6};

	static int digit_select = 0;
	// msec2_count는 밀리초 단위이므로, 50ms마다 프레임이 전환되도록 설정 (12 프레임 애니메이션)
	int frame = (msec2_count / 50) % 12;
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT = 0x08;
		FND_DATA_PORT = fnd_font0[frame];
		break;
		case 1:
		FND_DIGIT_PORT = 0x04;
		FND_DATA_PORT = fnd_font1[frame];
		break;
		case 2:
		FND_DIGIT_PORT = 0x02;
		FND_DATA_PORT = fnd_font2[frame];
		break;
		case 3:
		FND_DIGIT_PORT = 0x01;
		FND_DATA_PORT = fnd_font3[frame];
		break;
	}
	digit_select++;
	digit_select %= 4;
}

void fnd_stop_display(void)
{
	uint8_t fnd_font0 = 0x8c;
	uint8_t fnd_font1 = 0xc0;
	uint8_t fnd_font2 = 0x87;
	uint8_t fnd_font3 = 0x92;

	static int digit_select = 0;
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT = 0x08;
		FND_DATA_PORT = fnd_font0;
		break;
		case 1:
		FND_DIGIT_PORT = 0x04;
		FND_DATA_PORT = fnd_font1;
		break;
		case 2:
		FND_DIGIT_PORT = 0x02;
		FND_DATA_PORT = fnd_font2;
		break;
		case 3:
		FND_DIGIT_PORT = 0x01;
		FND_DATA_PORT = fnd_font3;
		break;
	}
	digit_select++;
	digit_select %= 4;
	
	msec2_count = temp;
}