

#include "pwm.h"
#include "buzzer.h"

#define MAX_SPEED_FACTOR 10  // 최대 속도 배율 설정
#define MIN_SPEED_FACTOR 1   // 최소 속도 배율
#define PRESET_TIME 21       // 각 phase 동작 시간 (20초 동작 후 약 1초 STOP 유지)
#define SUBPHASE_TIME 3      // Phase2에서 회전 방향 전환 간격 (예: 3초)

// button 함수
extern void init_button(void);
extern int get_button(int button_num, int button_pin);
// fnd 함수
extern void fnd_display(void);            // FND1: 카운트다운 및 선택 phase 번호 표시
extern void fnd_big_circle_forward(void); // FND2: 정방향 애니메이션
extern void fnd_big_circle_backward(void);// FND2: 역방향 애니메이션
extern void fnd_stop_display(void);       // FND2: STOP 메시지 출력

void init_timer3(void);
void L298N_pwm_fan_fnd_control_main(void);
void init_L298N(void);

extern volatile int msec_count;
extern volatile int sec_count;
extern volatile int msec2_count;
extern volatile int sec2_count;
extern volatile int msec2_speed_factor;
extern volatile int display_time;

volatile int temp = 0;

// phase: 현재 실행하는 phase (1,2,3 실행 중이면 해당 phase 실행, 0이면 idle 상태)
// selected_phase: idle 상태에서 버튼1으로 선택한 phase (1,2,3)
// manual_mode: 0이면 기본(default) sequence, 1이면 수동 선택 모드 (선택 phase만 실행)
uint8_t phase = 0;
uint8_t selected_phase = 0;
uint8_t manual_mode = 0;

// system_on: 0이면 전체 전원 OFF, 1이면 전체 전원 ON (버튼3 토글)
uint8_t system_on = 0;

void init_timer3(void)
{
	DDRE |= (1 << 3) | (1 << 5);      // PE3, PE5 출력 (PE4는 사용 중)
	TCCR3A |= 1 << WGM30;
	TCCR3B |= 1 << WGM32;
	TCCR3A |= 1 << COM3C1;
	TCCR3B |= (1 << CS31) | (1 << CS30);  // 분주비 64
	OCR3C = 0;  // 초기 PWM 값 0
}

void init_L298N(void)
{
	DDRF |= (1 << 6) | (1 << 7);
	PORTF &= ~((1 << 6) | (1 << 7));
	PORTF |= (1 << 6);  // 초기 정회전 설정
}
void L298N_pwm_fan_fnd_control_main(void)
{
	uint8_t forward = 1;    // 1: 정회전, 0: 역회전
	unsigned int phase_start_sec = 0; // 현재 phase 시작 시간

	init_button();
	init_timer3();
	init_L298N();
	init_buzzer();

	// LED 핀 초기화: LED0 → PA0, LED1 → PA1, LED2 → PA2
	DDRA |= (1 << 0) | (1 << 1) | (1 << 2);

	while(1)
	{
		// 버튼3: 전체 전원 토글 (누를 때마다 system_on 값 반전)
		if(get_button(BUTTON3, BUTTON3PIN)) {
			beep(100);
			system_on = !system_on;
			_delay_ms(200); // 디바운스
		}
		
		// 전체 전원이 꺼져 있으면 모든 모듈 OFF 처리 후 나머지 코드 실행 무시
		if(system_on == 0) {
			OCR3C = 0;              // 모터 PWM OFF
			fnd_stop_display();     // FND에 STOP 메시지 출력
			PORTA &= ~((1 << 0) | (1 << 1) | (1 << 2)); // 모든 LED OFF
			phase = 0;              // idle 상태
			selected_phase = 0;
			continue;
		}
		
		if(phase == 0) {  // Idle 상태 (선택 모드)
			// 모터 OFF, 모든 LED OFF, FND1에는 현재 선택된 phase 번호 표시 (없으면 0)
			OCR3C = 0;
			display_time = selected_phase;
			fnd_stop_display();
			PORTA &= ~((1 << 0) | (1 << 1) | (1 << 2));

			// 버튼0: 기본 실행 (default sequence) → Phase1부터 순차 실행
			if(get_button(BUTTON0, BUTTON0PIN)) {
				beep(100);
				phase = 1;
				manual_mode = 0;  // 기본 모드
				phase_start_sec = sec2_count;
				display_time = PRESET_TIME;
				forward = 1;  // 정회전
				PORTF &= ~((1 << 6) | (1 << 7));
				PORTF |= (1 << 6);
				OCR3C = 250;
				PORTA |= (1 << 0);  // LED0 ON
				_delay_us(100);     // 디바운스
			}
			// 버튼1: 수동 선택 모드에서 phase 선택 (1~3)
			if(get_button(BUTTON1, BUTTON1PIN)) {
				beep(100);
				selected_phase++;
				if(selected_phase > 3) selected_phase = 1;
				_delay_us(100);  // 디바운스
			}
			// 버튼2: 수동 선택 모드에서 선택한 phase로 실행 (단, 선택 phase가 0이 아니면)
			if(get_button(BUTTON2, BUTTON2PIN)) {
				beep(100);
				if(selected_phase != 0) {
					phase = selected_phase;
					manual_mode = 1;  // 수동 모드
					phase_start_sec = sec2_count;
					display_time = PRESET_TIME;
					if(phase == 1) {
						forward = 1;
						PORTF &= ~((1 << 6) | (1 << 7));
						PORTF |= (1 << 6);
						OCR3C = 250;
						PORTA |= (1 << 0);  // LED0 ON
						} else if(phase == 2) {
						forward = 1;
						PORTF &= ~((1 << 6) | (1 << 7));
						PORTF |= (1 << 6);
						OCR3C = 250;
						PORTA |= (1 << 1);  // LED1 ON
						} else if(phase == 3) {
						forward = 0;
						PORTF &= ~((1 << 6) | (1 << 7));
						PORTF |= (1 << 7);
						OCR3C = 250;
						PORTA |= (1 << 2);  // LED2 ON
					}
					_delay_us(100);  // 디바운스
				}
			}
		}
		else if(phase == 1) {  // Phase1: 20초 동안 연속 정회전
			int elapsed = sec2_count - phase_start_sec;
			int remaining = PRESET_TIME - elapsed;
			if(remaining > 0) {
				display_time = remaining;
				} else {
				display_time = 0;
				fnd_stop_display();
				OCR3C = 0;
				PORTA &= ~(1 << 0);
				beep(100);
				_delay_us(100);
				// 기본 모드: 자동으로 Phase2로 전환, 수동 모드: 바로 idle 복귀
				if(manual_mode == 0) {
					phase = 2;
					phase_start_sec = sec2_count;
					display_time = PRESET_TIME;
					forward = 1;
					PORTF &= ~((1 << 6) | (1 << 7));
					PORTF |= (1 << 6);
					OCR3C = 250;
					PORTA |= (1 << 1);  // LED1 ON
					} else {
					phase = 0;
					selected_phase = 0;
				}
			}
			fnd_big_circle_forward();
		}
		else if(phase == 2) {  // Phase2: 20초 동안 5초 간격으로 정/역 전환
			int elapsed = sec2_count - phase_start_sec;
			int remaining = PRESET_TIME - elapsed;
			if(remaining > 0) {
				display_time = remaining;
				int subphase = (elapsed / SUBPHASE_TIME) % 2;  // 0 또는 1
				if(subphase == 0) {
					if(forward != 1) {
						forward = 1;
						PORTF &= ~((1 << 6) | (1 << 7));
						PORTF |= (1 << 6);
					}
					} else {
					if(forward != 0) {
						forward = 0;
						PORTF &= ~((1 << 6) | (1 << 7));
						PORTF |= (1 << 7);
					}
				}
				OCR3C = 250;
				if(forward)
				fnd_big_circle_forward();
				else
				fnd_big_circle_backward();
				} else {
				display_time = 0;
				fnd_stop_display();
				OCR3C = 0;
				PORTA &= ~(1 << 1);
				beep(100);
				_delay_us(100);
				if(manual_mode == 0) {  // 기본 모드: 자동 전환 → Phase3
					phase = 3;
					phase_start_sec = sec2_count;
					display_time = PRESET_TIME;
					forward = 0;
					PORTF &= ~((1 << 6) | (1 << 7));
					PORTF |= (1 << 7);
					OCR3C = 250;
					PORTA |= (1 << 2);  // LED2 ON
					} else {
					phase = 0;
					selected_phase = 0;
				}
			}
		}
		else if(phase == 3) {  // Phase3: 20초 동안 연속 역회전
			int elapsed = sec2_count - phase_start_sec;
			int remaining = PRESET_TIME - elapsed;
			if(remaining > 0) {
				display_time = remaining;
				} else {
				display_time = 0;
				fnd_stop_display();
				OCR3C = 0;
				PORTA &= ~(1 << 2);
				beep(100);
				_delay_us(100);
				// Phase3 종료 후, 기본 모드와 수동 모드 모두 idle로 복귀
				phase = 0;
				selected_phase = 0;
			}
			fnd_big_circle_backward();
		}
		
		// FND1: 동작 중에는 남은 시간 카운트, idle 상태에서는 선택 phase 번호 표시
		fnd_display();
		_delay_us(500);
	}
}

