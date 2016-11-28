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


unsigned char led;		// PB4 (0/1) - (not present/present) on board
unsigned char present;	// present flag sent from Pi
unsigned char wake_up;	// time to wake up flag sent from Uno
unsigned char trigger;  // flag sent from ATmega to Uno to turn off alarm
unsigned char dist;		// flag to allow scoring to count
unsigned char ir_read;	// read from the IR receiver sensor

unsigned short user_dist; // checks the distance of the user from the sensor

#define RPI 0
#define UNO 1
#define DIST_PERIOD 50

// Create State Machine enumeration here
enum led_states {INIT, WAIT, WAKEUP, PRESENT, PLAY} led_state;
	
enum dist_states {DIST_INIT, DIST_WAIT} dist_state;

void Dist_InitFct() {
	EnableDistance();
	dist_state = DIST_INIT;
}

void Dist_TickFct() {
	  unsigned char distance;
	  const unsigned char THRESH = 15;  // distance to trigger demo LED
	  const unsigned char LED_PIN = 5;  // LED pin for demo
	  //Actions
	  switch (dist_state) {
		  case DIST_INIT:
		  break;
		  case DIST_WAIT:
		  
			if (present == 0x01) {
				distance = PingCM();

				if (distance >= THRESH) {
					DIST_PORT |= (1<<LED_PIN);
					dist = 0x01;
				}
				else {
					DIST_PORT &= ~(1<<LED_PIN);
					dist = 0x00;
				} 
			}
		  break;
		  default:
		  break;
	  }
	  //Transitions
	  switch (dist_state) {
		  case DIST_INIT:
		  dist_state = DIST_WAIT;
		  break;
		  case DIST_WAIT:
		  break;
		  default:
		  dist_state = DIST_INIT;
		  break;
	  }
}


void Dist_TaskFct() {
	Dist_InitFct();
	for(;;)
	{
		Dist_TickFct();
		vTaskDelay(DIST_PERIOD);
	}
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
				led_state = PRESENT;
			}
			else if (present == 0x00) {
				led_state = WAKEUP;
			}

			break;

		case PRESENT:

			PORTA = 0x04;
			
			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}

			if (present == 0x01) { // user is present on board
				if (dist == 0x01) {
					led_state = PLAY;
				}
				else if (dist == 0x00) {
					led_state = PRESENT;
				}

			}
			else if (present == 0x00) {
				led_state = WAKEUP;
			}

			break;

		case PLAY:
			PORTA = 0x08;
			
			if (USART_HasReceived(RPI)) {
				present = USART_Receive(RPI);
			}

			if (present == 0x01) { // user is present on board
				if (dist == 0x01) {
					
					if (ir_read == 0x01) {
						trigger = 0x01;
						USART_Send(trigger, UNO);
					}
					else if (ir_read == 0x00) {
						trigger = 0x00;
					}
					
					led_state = PLAY;
				}
				else if (dist == 0x00) {
					led_state = PRESENT;
				}

			}
			else if (present == 0x00) {
				led_state = WAKEUP;
			}


			break;

		default:
			led_state = INIT;
			break;
	}

	switch (led_state) { // Actions
		case INIT:
			break;

		case WAIT:
			break;

		case WAKEUP:
			break;

		case PRESENT:
			break;
			
		case PLAY:
			break;

		default:
			break;
	}
}

void TaskFct() {
	InitFct();
	while (1) {
		TickFct();
		vTaskDelay(50);
	}
}

void PulseFct(unsigned portBASE_TYPE Priority) {
	// Create Tasks here
	xTaskCreate(TaskFct, (signed portCHAR *)"TaskFct", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	//xTaskCreate(Dist_TaskFct, (signed portCHAR *)"Dist_TaskFct", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	DDRA = 0xFF;	PORTA = 0x00;	// debug state machines
	//DDRB = 0xFD;	PORTB = 0x02;	// Distance sensor + LED
	DDRC = 0x00;	PORTC = 0xFF;	// IR receivers
	initUSART(RPI);	initUSART(UNO);	// USART0 - pi_atmega	USART1 - atmega_uno

	PulseFct(1);
	vTaskStartScheduler();

	return 0;
}
