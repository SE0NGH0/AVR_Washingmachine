# 🧺 Washing Machine Project  
**With ATmega128A**  

---

## 📌 Project Overview

- AVR ATmega128A 마이크로컨트롤러의 응용 능력 향상을 목표로 한 임베디드 프로젝트
- 실제 세탁기 동작 시퀀스(세탁 → 헹굼 → 탈수)를 구현
- 다양한 외부 장치 제어 (LED, FND, MOTOR, BUZZER, BUTTON 등)를 통해 가시적 인터페이스 제공

---

## 🔩 Hardware Components

| 부품명           | 용도                |
|------------------|---------------------|
| ATmega128A       | MCU (중앙 제어)     |
| L298N            | 모터 구동 드라이버  |
| DC Motor         | 세탁통 회전용       |
| LED              | 각 단계 진행 상태 표시 |
| BUZZER           | 알림음 출력         |
| FND (4-digit)    | 남은 시간 및 애니메이션 표시 |
| BUTTON (4개)     | 모드 선택 및 실행   |
| Trimmer          | 세기 조절(옵션)     |

---

## 🧠 Mode & State Logic

- **Button 0**: 자동 모드 시작 (세탁 → 헹굼 → 탈수 순차 진행)  
- **Button 1**: 수동 모드 (단계 선택: 1~3 반복 순환)  
- **Button 2**: 선택한 단계 실행  
- **Button 3**: 전원 On/Off 및 긴급 정지  

> FSM(상태 천이도)을 기반으로 Phase0~Phase3까지 각 단계 관리

---

## 🔧 Core Features

| 기능              | 설명 |
|-------------------|------|
| **Phase 0**       | 사용자 입력 대기 (모터 정지, LED OFF, FND: 선택 단계 표시) |
| **Phase 1** - 세탁 | 일정 시간 동안 정방향 회전 (FND 시간 표시 + 애니메이션) |
| **Phase 2** - 헹굼 | 일정 시간마다 정/역방향 교대 (subphase 로직) |
| **Phase 3** - 탈수 | 일정 시간 동안 역방향 회전 |

- 각 단계 종료 후 자동 모드면 다음 단계로 자동 전환  
- 수동 모드는 단계 종료 후 대기 상태로 복귀  
- BUZZER는 버튼 클릭, 단계 종료 시 효과음 출력

---

## 💻 Code Highlights

### `pwm.c`

- 타이머3 설정으로 PWM 제어
- 모터 정·역방향 제어 및 속도 조절
- Phase 상태 및 시간 기록, 전원 관리 등 구현

### `fnd.c`

- 4자리 FND 다중화 및 숫자 표시
- 애니메이션 효과(정방향/역방향 프레임 기반) 구현
- "STOP" 문자 표시 기능 포함

### `speaker.c`

- `Music_Player()`를 통해 종료음 출력
- 현재는 블로킹 방식으로 동작 → 추후 인터럽트 방식 개선 필요

---

## 📽 Demo Video

[🔗 시연 영상 보기](https://youtube.com/shorts/i7M0yNZkVg0)

---

## 📊 Test & Verification

- UART 오실로스코프를 통한 상태 표시 확인 ("STOP" 출력 등)
- PWM 파형 분석을 통한 모터 구동 상태 확인

---

## ⚠️ Problems & Solutions

| 문제점 | 개선 방향 |
|--------|-----------|
| 블로킹 delay 사용으로 인해 FND 밝기 저하, 동시 동작 충돌 | 타이머 인터럽트 기반 비동기 처리로 개선 |
| BUZZER와 FND 동시 사용 시 깜빡임 발생 | 타이머 분리 또는 스케줄링 필요 |
| BUTTON 처리 중 FND 지연 출력 발생 | debounce 및 상태 분기 처리 보완 필요 |

---

## 🔧 개선 아이디어

- 타이머 인터럽트를 활용한 정확한 모듈 제어
- 모터 속도 및 동작 시간 가변 제어
- 블루투스, 초음파 센서 등 기능 확장

---
