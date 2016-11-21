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

// Create State Machine enumeration here

void InitFct(){
	// Initialize State Machines here
}

void TickFct(){
	// Transitions

	// Actions
}

void TaskFct()
{
	// InitFct()
	for(;;)
	{
		// TickFct()
		vTaskDelay(100);
	}
}

void PulseFct(unsigned portBASE_TYPE Priority)
{
	// Create Tasks here
	xTaskCreate(/*TaskFct*/, (signed portCHAR *)/*"TaskFct"*/, configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	while(1)
	{
		// TODO: Please write your application code
	}
}
