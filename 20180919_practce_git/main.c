/*
 * 20180919_practce_git.c
 *
 * Created: 2018-09-19 오전 10:49:31
 * Author : kccistc
 */ 

#define F_CPU	16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "UART0.h"
#include "I2C_LCD.h"
#include "FND.h"

#define PRESCALER	1024
#define ULTRA_PORT	PORTG
#define ULTAR_DDR	DDRG
#define ULTRA_PIN	PING
#define ULTRA_TRIG	4
#define ULTRA_ECHO	3

static FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
static FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

void UltraSonic_init(void)
{
	TCCR1B |= (1 << CS12) | (1 << CS10); //16비트 타이머 1번 타이머/카운터 분주비를 1024로 설정
	
	ULTAR_DDR |= (1 << ULTRA_TRIG);
	ULTAR_DDR &= ~(1 <<ULTRA_ECHO);
}

void UltraSonic_Trigger(void)
{
	//트리거 핀으로 펄스 출력
	ULTRA_PORT &= ~(1 << ULTRA_TRIG);
	_delay_us(1);
	ULTRA_PORT |= (1 << ULTRA_TRIG);
	_delay_us(10);
	ULTRA_PORT &= ~(1 << ULTRA_TRIG);
}

uint8_t measure_distance(void)
{
	UltraSonic_Trigger();
	
	
	TCNT1 = 0;
	while(!(ULTRA_PIN & (1 << ULTRA_ECHO))) // echo pin이 high가 될 때까지 대기
	{
		if(TCNT1 > 65000)
		return 0;
	}
	
	TCNT1 = 0;
	while(ULTRA_PIN & (1 << ULTRA_ECHO))
	{
		if(TCNT1 > 650000)
		{
			TCNT1 = 0;
			break;
		}
	}
	
	double pulse_width = 1000000.0*TCNT1*PRESCALER/F_CPU;
	
	return pulse_width / 58;
}

int main(void)
{
	uint8_t distance;
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	UART0_init();
	UltraSonic_init();
	FND_init();
	
	/* Replace with your application code */
	while (1)
	{
		distance = measure_distance();
		
		printf("Distance: %d cm\r\n", distance);
		_delay_ms(500);
		
		displayNumber(distance);
		_delay_ms(10);
	}
	return 0;
}


