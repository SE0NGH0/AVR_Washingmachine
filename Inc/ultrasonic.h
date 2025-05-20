/*
 * ultrasonic.h
 *
 * Created: 2025-03-12 오후 2:49:07
 *  Author: microsoft
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us
#include <avr/interrupt.h> // interrupt
#include <stdio.h>
#include <string.h>
// TRIG 세팅
#define TRIG      4
#define TRIG_PORT   PORTG
#define TRIG_DDR   DDRG

// ECHO 세팅
#define ECHO      4
#define ECHO_PIN   PINE   // External INT 4
#define ECHO_DDR   DDRE


void init_ultrasonic();
void trigger_ultrasonic();
void ultrasonic_distance_check();


#endif /* ULTRASONIC_H_ */