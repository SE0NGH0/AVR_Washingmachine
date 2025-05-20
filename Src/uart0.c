/*
 * uart0.c
 *
 * Created: 2025-03-10 오전 10:32:31
 *  Author: microsoft
 */ 
#include "uart0.h"
#include <string.h>							// strcpy, strcnp, stcmp 등등

extern void led_all_on(void);
extern void led_all_off(void);
extern void shift_left_ledon(void);
extern void shift_right_ledon(void);
extern void shift_left_keep_ledon(void);
extern void shift_right_keep_ledon(void);
extern void flower_on(void);
extern void flower_off(void);
						
void init_uart0(void);
void UART0_transmit(uint8_t data);


/*
   PC comportmaster로 부터 1byte기 들어 올때 마다 이속(ISR(USART0_RX_vect)으로 들어 온다. (RX INT)
   예)  led_all_on\n ==> 11번 이곳으로 들어 온다 
        led_all_off\n
*/
volatile uint8_t rx_msg_received=0;
ISR(USART0_RX_vect)
{
	volatile uint8_t rx_data;
	volatile static int i=0;
	
	rx_data = UDR0;  // uart0의 H/W register(UDR0)로 부터 1byte를 읽어 들인다. 
	                 // rx_data = UDR0;를 실행하면 UDR0의 내용이 빈다.(empty)
	if (rx_data == '\n')
	{
		rx_buff[rear++][i] = '\0';		
		rear %= COMMAND_NUMBER;								// rear : 0 ~ 9
		i = 0;												// 다음 string을 저장하기 위한 1차원 index값을 0으로
		// !!!! rx_buff queue full check 하는 logic 추가
	}
	else
	{
		rx_buff[rear][i++] = rx_data;
		// COMMAND_LENGTH를 check하는 logic 추가
	}
}
/*
  1. 전송속도: 9600bps : 총글자수 :9600/10bit ==> 960자 
     ( 1글자를 송.수신 하는데 소요 시간 : 약 1ms)
  2. 비동기 방식(start/stop부호 비트로 데이터를 구분)
  3. RX(수신) : 인터럽트 방식으로 구현 
*/

void init_uart0(void)
{
	// 1. 9600bps로 설정
	UBRR0H = 0x00;
	UBRR0L = 207;  // 9600bps P219 표9-9
	// 2. 2배속 통신  표9-1
	UCSR0A |= 1 << U2X0;  // 2배속 통신 
	UCSR0C |= 0x06;   // 비동기/data8bits/none parity
	// P215 표9-1
	// RXEN0 : UART0로 부터 수신이 가능 하도록 
	// TXEN0 : UART0로 부터 송신이 가능 하도록 
	// RXCIE0 : UART0로 부터 1byte가 들어오면(stop bit가 들어 오면)) rx interrupt를 발생 시켜라
	UCSR0B |= 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0;
	
}

// UART0로 1byte를 전송 하는 함수 (polling방식)
void UART0_transmit(uint8_t data)
{
	// 데이터 전송 중이면 전송이 끝날떄 까지 기다린다. 
	while ( !(UCSR0A & 1 << UDRE0))
		;   // no operation
	UDR0 = data;  // data를 H/W전송 register에 쏜다. 
}

// 현재 실행할 함수를 저장할 포인터 변수 (초기값은 NULL)
void (*current_function)() = NULL;

void pc_command_processing(void)
{
	if (front != rear)  // UART 수신 버퍼에 데이터가 있으면
	{
		printf("Received Command: %s\n", rx_buff[front]);  // 입력된 명령 출력

		// 명령어 배열 및 함수 포인터 배열
		static const char* function_names[] =
		{
			"led_all_on",
			"led_all_off",
			"shift_left_ledon",
			"shift_right_ledon",
			"shift_left_keep_ledon",
			"shift_right_keep_ledon",
			"flower_on",
			"flower_off"
		};

		void (*functions[])() =
		{
			led_all_on,
			led_all_off,
			shift_left_ledon,
			shift_right_ledon,
			shift_left_keep_ledon,
			shift_right_keep_ledon,
			flower_on,
			flower_off
		};

		// 명령어 비교 및 함수 실행
		for (int i = 0; i < sizeof(function_names) / sizeof(function_names[0]); i++)			// function_names안에 함수 이름의 갯수만큼 비교한다.
		{
			if (strncmp(rx_buff[front], function_names[i], strlen(function_names[i])) == 0)		// buffer에 있는 함수의 이름과 function_names의 함수 이름과 비교한다.
			{
				printf("Setting function: %s\n", function_names[i]);							// 함수를 실행한다고 보여준다.
				current_function = functions[i];												// 현재 실행할 함수 설정
				break;
			}
		}

		front++;
		front %= COMMAND_NUMBER;  // 큐의 순환 유지
	}
}