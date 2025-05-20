/*
 * led.c
 *
 * Created: 2025-03-05 오전 10:21:53
 *  Author: microsoft
 */ 

#include "led.h"
#define FUNC_NUMER 6

int led_main(void);   // 선언
void led_all_on(void);
void led_all_off(void);
void shift_left_ledon(void);
void shift_right_ledon(void);
void shift_left_keep_ledon(void);
void shift_right_keep_ledon(void);
void flower_on(void);
void flower_off(void);
void state_transition(void);
void init_led(void);
void make_pwm_led_control(void);

extern volatile int msec_count;
extern volatile int ultrasonic_dist;

int state=0;
void (*fp[]) () = 
{
	shift_left_ledon,
	shift_right_ledon,
	shift_left_keep_ledon,
	shift_right_keep_ledon,
	flower_on,
	flower_off
};


int led_main(void)   // 정의 
{
	init_led();

	while(1)
	{
		fp[state]();
	}
}

void init_led(void)
{
	DDRA = 0xff;  
}

void flower_on(void)
{
	static int i=0;

	if (msec_count >= 100)
	{
		msec_count=0;
		if (i >= 4)
		{
			i=PORTA=0x00;
			state_transition();   // state를 천이 to shift_right_keep_ledon
		}
		else
		{
			PORTA |= 0x10 << i | 0x08 >> i++;
		}
	}
}

void flower_off(void)
{
	unsigned char h=0xf0;
	unsigned char l=0x0f;
	
	static int i=0;

	if (msec_count >= 30)
	{
		msec_count=0;
		if (i >= 4)
		{
			i=PORTA=0x00;
			state_transition();   // state를 천이 to shift_right_keep_ledon
		}
		else
		{
			PORTA = ( ((h >> i) & 0xf0) | ((l << i) & 0x0f));
			i++;
		}
	}
}

void shift_left_keep_ledon(void)
{
	static int i=0;

	if (msec_count >= 30)
	{
		msec_count=0;
		if (i >= 8)
		{
			i = PORTA = 0x00;
			state_transition();  // state를 천이 to shift_right_keep_ledon
		}
		else
		{
			PORTA =PORTA |= 0b00000001 << i++;
		}
	}
}

void shift_right_keep_ledon(void)
{
	static int i=0;

	if (msec_count >= 30)
	{
		msec_count=0;
		if (i >= 8)
		{
			i=PORTA=0x00;
			state_transition();   // state를 천이 to shift_right_keep_ledon
		}
		else
		{
			PORTA |= 0b10000000 >> i++;
		}
	}
}

void shift_left_ledon(void)
{
   // (1)for문 제거  (2) _delay_ms(30) 제거
   static int i=0;
   
   if (msec_count >= 30)
   {
	   msec_count=0;
	   if (i >= 8)
	   {
		   i=PORTA=0;
		   state_transition();
	   }
	   else
	   {
		   PORTA = 0b00000001 << i++;  // (1) PORTA = 0b00000001 << i  (2) i++
	   }
   } 	
}

void shift_right_ledon(void)
{
	static int i=0;

	if (msec_count >= 30)
	{
		msec_count=0;
		if (i >= 8)
		{
			i=PORTA=0x00;
			state_transition();   // state를 천이 to shift_left_keep_ledon
		}
		else
		{
			PORTA = 0b10000000 >> i++;
		}
	} 
}

void led_all_on(void)
{
	PORTA = 0xff;
}

void led_all_off(void)
{
	PORTA = 0x00;
}

void state_transition(void)
{
	state++;
	state %= FUNC_NUMER;
}

void distance_led()
{
	int distance = 0;
	
	distance = ultrasonic_dist / 58;

	if(distance > 0 && distance < 3)
	{
		PORTA = 0x01;
	}
	else if(distance > 2 && distance < 6)
	{
		PORTA = 0x03;
	}
	else if(distance > 5 && distance < 9)
	{
		PORTA = 0x07;
	}
	else if(distance > 8 && distance < 12)
	{
		PORTA = 0x0f;
	}
	else if(distance > 11 && distance < 14)
	{
		PORTA = 0x1f;
	}
	else if(distance > 13 && distance < 16)
	{
		PORTA = 0x3f;
	}
	else if(distance > 15 && distance < 18)
	{
		PORTA = 0x7f;
	}
	else
	{
		PORTA = 0xff;
	}
}

void make_pwm_led_control(void)
{
	int dim = 0;		// led 밝기 조절 변수
	int direction = 1;	// 1: 밝기 증가 모드, -1: 밝기 감소 모드
	
	DDRA = 0xff;
	PORTA = 0xff;		// led all on
	
	// 1. dark --> bright 2. bright --> dark 두 가지 구현
	while(1)
	{
		led_on_pwm(dim);
		dim += direction;
		if(dim == 255)
			direction = -1;
		else if(dim == 0)
			direction = 1;
	}
}

void led_on_pwm(int dim)
{
	PORTA = 0xff;
	
	for(int i = 0; i <256; i++)
	{
		if(i > dim)
		{
			PORTA = 0;				// duty cycle이 max를 넘어 가면 led all off
		}
		_delay_us(20);
	}
}
