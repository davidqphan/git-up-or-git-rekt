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
#include "distance.h"

// Create State Machine enumeration here
enum led_states {INIT, WAIT, WAKEUP, READY, DIST, PLAY} led_state;

unsigned char led;		// PB4 (0/1) - (not present/present) on board
unsigned char present;	// present flag sent from Pi
unsigned char wake_up;	// time to wake up flag sent from Uno
unsigned char trigger;  // flag sent from ATmega to Uno to turn off alarm
unsigned char distance; // flag to allow scoring to count
unsigned char ir_read;	// read from the IR receiver sensor

unsigned short user_dist; // checks the distance of the user from the sensor

#define RPI 0
#define UNO 1

unsigned char check_dist() {
	unsigned char flag = 0x00;
	user_dist = PingIN();
	if (user_dist >= 60) { // user is within legal distance
		PORTB |= (1<<DIST_TRIGGER);
		flag = 0x01;
	}
	else {
		PORTB &= ~(1<<DIST_TRIGGER);
		flag = 0x00;
	}
	return flag;
}


void InitFct() {
	// Initialize State Machines here
	led_state = INIT;
}

void TickFct() {
	
	ir_read = ~PINC & 0x01;	
	
	switch (led_state) { // transitions
		case INIT:
			led_state = WAIT;
			break;
			
		case WAIT:
		
			PORTA = 0x01;
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
		
			PORTA = 0x02;
				
			USART_Send(wake_up, RPI);	

			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}
			
			if (present == 0x01) {
				led_state = READY;
			}
			else if (present == 0x00) {
				led_state = WAKEUP;
			}
			
			break;
			
		case READY:
		
		
			PORTA = 0x04;
			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}
			
			if (present == 0x01) { // user is present on board
	
				distance = check_dist();

				if (distance == 0x01) {
					led_state = DIST;
				}
				else if (distance == 0x00) {
					led_state = READY;
				}
				
			}
			else if (present == 0x00) {
				led = 0x00;
				led_state = WAKEUP;
			}
			
			break;
			
		case DIST:
				
			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}
						
			if (present == 0x01) { // user is present on board
							
				distance = check_dist();
							
				if (distance == 0x01) {
					led_state = PLAY;
				}
				else if (distance == 0x00) {
					led_state = READY;
				}
							
			}
			else if (present == 0x00) {
				led = 0x00;
				led_state = WAKEUP;
			}
						
			break;
			
		case PLAY:
			
			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}
					
			if (present == 0x01) { // user is present on board			
				distance = check_dist();
				if (distance == 0x01) {
					
					if (ir_read == 0x01) { // ball triggered the IR receiver
						trigger = 0x01;	// tells UNO to turn off alarm clock	
						USART_Send(trigger, UNO);
						led_state = WAIT;
					}
					else {
						trigger = 0x00;
						USART_Send(trigger, UNO);
						led_state = PLAY;
					}

					
				}
				else if (distance == 0x00) {
					led_state = DIST;
				}
						
			}
			else if (present == 0x00) {
				led = 0x00;
				led_state = WAKEUP;
			}
					
			break;	
		
		default:
			led_state = INIT;
			break;
	}
	
	switch (led_state) { // Actions
		case INIT:
			PORTA = 0x00;
			break;
		
		case WAIT:
			PORTA = 0x01;
			break;
		
		case WAKEUP:
			PORTA = 0x02;
			break;
		
		case READY:
			PORTA = 0x04;
			break;
			
		case DIST:
			PORTA = 0x10;
			break;
			
		case PLAY:
			PORTA = 0x20;
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
	DDRA = 0xFF;	PORTA = 0x00;	// debug state machines
	DDRB = 0xFD;	PORTB = 0x02;	// Distance sensor + LED
	DDRC = 0x00;	PORTC = 0xFF;	// IR receivers
	initUSART(RPI);	initUSART(UNO);	// USART0 - pi_atmega	USART1 - atmega_uno
	
	PulseFct(1);
	vTaskStartScheduler();
	
	return 0;
}
