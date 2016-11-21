/*	dphan015_gitup.c - 11/21/2016 1:27:45 PM
 *	Name & E-mail:  - David Phan - dphan015@ucr.edu
 *	CS Login: dphan015
 *	Lab Section: 022
 *	Assignment: Final Project
 *	Exercise Description: Git Up or Git Rekt
 *
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
#include "usart_ATmega1284.h"

// Create State Machine enumeration here
enum led_states {INIT, OFF, ON} led_state;

unsigned char tmpB;
unsigned char led;
unsigned char trigger;

void InitFct(){
	// Initialize State Machines here
	led_state = INIT;
}

void TickFct(){
	
	tmpB = ~PINB & 0x01;
	
	switch (led_state) { // transitions
		case INIT:
		led_state = OFF;
		break;
		
		case OFF:
		if(tmpB == 1){
			led_state = ON;
		}
		else{
			led_state = OFF;
		}
		break;
		
		case ON:
		if(tmpB == 1){
			led_state = ON;
		}
		else{
			led_state = OFF;
		}

		break;
		default:
		
		break;
	}
	
	switch (led_state) { // Actions
		case INIT:
		led = 0x00;
		trigger = 0x00;
		break;
		
		case OFF:
		led = 0x00;
		break;
		
		case ON:
		led = 0x01;
		trigger = 0x01;
		if(USART_IsSendReady(0)) {
			USART_Send(trigger,0);
		}
		break;
		
		default:
		break;
	}
	
	PORTA = led;
	
}

void TaskFct()
{
	InitFct();
	for(;;)
	{
		TickFct();
		vTaskDelay(1);
	}
}

void PulseFct(unsigned portBASE_TYPE Priority)
{
	// Create Tasks here
	xTaskCreate(TaskFct, (signed portCHAR *)"TaskFct", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRA = 0xFF;	PORTA = 0x00;	// LED output
	DDRB = 0x00;	PORTB = 0xFF;	// IR input
	
	initUSART(0);
	
	PulseFct(1);
	vTaskStartScheduler();
	
	
	return 0;
}
