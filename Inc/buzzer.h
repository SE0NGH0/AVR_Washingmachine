/*
 * buzzer.h
 *
 * Created: 2025-03-15 오후 1:22:37
 *  Author: microsoft
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_

#include <util/delay.h>

#define BUZZER_PIN 3
#define BUZZER_PORT PORTE
#define BUZZER_DDR  DDRE

// 부저를 초기화합니다.
void init_buzzer(void) {
	BUZZER_DDR |= (1 << BUZZER_PIN);       // 부저 핀 출력으로 설정
	BUZZER_PORT &= ~(1 << BUZZER_PIN);     // 부저 OFF (low)
}

// duration_ms 동안 부저로 beep음을 발생시킵니다.
// 예: 2kHz 사각파 (약 250us high, 250us low)
void beep(uint16_t duration_ms) {
	// 500us period = 2kHz, 따라서 duration_ms*1000/500번 반복
	uint16_t cycles = (duration_ms * 1000UL) / 500;
	for(uint16_t i = 0; i < cycles; i++){
		BUZZER_PORT |= (1 << BUZZER_PIN);
		_delay_us(250);
		BUZZER_PORT &= ~(1 << BUZZER_PIN);
		_delay_us(250);
	}
}


#endif /* BUZZER_H_ */