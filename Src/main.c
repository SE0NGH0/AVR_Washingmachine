
#define MAX_SPEED_FACTOR 10  // 최대 속도 배율 설정
#define MIN_SPEED_FACTOR 1   // 최소 속도 배율
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "button.h"

void init(void);

extern void init_led(void);
extern int led_main(void);
extern void init_button(void);
extern int get_button(int button_num, int button_pin);
extern void led_all_on(void);
extern void led_all_off(void);
extern void shift_left_ledon(void);
extern void shift_right_ledon(void);
extern void shift_left_keep_ledon(void);
extern void shift_right_keep_ledon(void);
extern void flower_on(void);
extern void flower_off(void);
extern int fnd_main(void);
extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern void (*current_function)(void);
extern init_ultrasonic();
extern void distance_ultrasonic();
extern void distance_led();
extern void fnd_distance_display();
extern void init_fnd(void);
extern void make_pwm_led_control(void);
extern void pwm_fan_control_main(void);
extern void L298N_pwm_fan_control_main(void);
extern void fnd_display(void);
extern volatile uint8_t rx_buff[80];
extern volatile uint8_t rx_msg_received;

volatile int msec_count = 0;
volatile int sec_count = 0;
volatile int msec2_count = 0;
volatile int sec2_count = 0;
volatile int msec2_speed_factor = 1;  // 기본 속도 1배
volatile uint32_t closed_check_timer = 0;
volatile uint32_t check_timer = 0; // 5000ms에 한번씩


//extern uint32_t sec_count;

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

int led_toggle=0;
ISR(TIMER0_OVF_vect)
{
	TCNT0=6;
	msec_count++;
	msec2_count += msec2_speed_factor;  // 버튼에 따라 속도 조절
	
	if(msec_count > 1000)
	{
		msec_count = 0;
		sec_count++;
	}
	if(msec2_count > 1000)
	{
		msec2_count = 0;
		sec2_count++;
	}
}


int main(void)
{
	init();
	
	//make_pwm_led_control();
	//pwm_fan_control_main();
	L298N_pwm_fan_fnd_control_main();
	
	while(1)
	{
	}
}


void init_timer0(void)
{
	TCNT0 = 6;
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00;
	TIMSK |= 1 << TOIE0;
}

void init(void)
{
	init_timer0();
	init_uart0();
	init_ultrasonic();
	
	stdout = &OUTPUT;
	sei();
	
	init_led();
	init_fnd();
}
