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
enum led_states {INIT, WAIT, WAKEUP, READY} led_state;

unsigned char led;		// PB4 (0/1) - (not present/present) on board
unsigned char present;	// present flag sent from Pi
unsigned char wake_up;	// time to wake up flag sent from Uno

#define RPI 0
#define UNO 1

void InitFct() {
	// Initialize State Machines here
	led_state = INIT;
}

void TickFct() {
	switch (led_state) { // transitions
		case INIT:
			led = 0x00;
			led_state = WAIT;
			break;
			
		case WAIT:
			if (USART_HasReceived(UNO)) {
				wake_up = USART_Receive(UNO);
			}
			
			if (wake_up == 0x01) {
				led_state = WAKEUP;
			}
			else if (wake_up == 0x00) {
				led_state = WAIT;
			}			
			break;
		
		case WAKEUP:
			USART_Send(wake_up, RPI);	

			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}
			
			if (present == 0x01) {
				led = 0x10;
				led_state = READY;
			}
			else if (present == 0x00) {
				led = 0x00;
				led_state = WAKEUP;
			}
			break;
			
		case READY:
			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}
			
			if (present == 0x01) {
				led = 0x10;
				led_state = READY;
			}
			else if (present == 0x00) {
				led = 0x00;
				led_state = WAKEUP;
			}
			break;
		
		default:
			break;
	}
	
	switch (led_state) { // Actions
		case INIT:
			break;
		
		case WAIT:
			break;
		
		case WAKEUP:
			PORTB = led;
			break;
		
		case READY:
			PORTB = led;
			break;
		
		default:
			break;
	}
}

void TaskFct() {
	InitFct();
	while (1) {
		TickFct();
		vTaskDelay(1);
	}
}

void PulseFct(unsigned portBASE_TYPE Priority) {
	// Create Tasks here
	xTaskCreate(TaskFct, (signed portCHAR *)"TaskFct", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	// DDRA = 0xFF;	PORTA = 0x00;	// LED bar to keep score
	DDRB = 0xFD;	PORTB = 0x02;	// Distance sensor + LED
	// DDRC = 0x00;	PORTC = 0xFF;	// IR receivers
	initUSART(RPI);	initUSART(UNO);	// USART0 - pi_atmega	USART1 - atmega_uno
	
	PulseFct(1);
	vTaskStartScheduler();
	
	return 0;
}
