#define F_CPU 8000000UL // Assume uC operates at 8MHz
#include <util/delay.h>

// Credits: Joel Gomez

#define DIST_REG  DDRB  // sensor DDR
#define DIST_PORT PORTB // sensor PORT
#define ECHO_PIN  3     // input to listen for echo ***DEPENDENT ON INT0***
#define TRIG_PIN  2     // output to start sensor polling
#define DIST_TRIGGER 5  // output pin to trigger when object detected

#define UNIT_IN 148   // uS/148 = inches
#define UNIT_CM 58    // uS/58 = centimeters

volatile unsigned short pulse = 0;
volatile char pulse_flag = 0;

ISR(INT2_vect) {
  if (pulse_flag == 1) {
    TCCR3B = 0;     // disable counter
    pulse = TCNT3;  // store counter memory
    TCNT3 = 0;      // reset counter memory
    pulse_flag = 0;
  }
  if (pulse_flag == 0) {
    TCCR3B |= (1<<CS31);  // enable counter
    pulse_flag = 1;
  }
}

// Flips the bits we need for this to work, only need to run once
void EnableDistance() {
  SREG |= (1<<7);       // enable global interrupts
  EIMSK |= (1<<INT2);   // enable external interrupt 0 (PD2)
  EICRA |= (1<<ISC21);  // set interrupt to trigger on logic change

  // set sensor trigger pin as output
  DIST_REG |= (1<<TRIG_PIN); DIST_PORT &= ~(1<<TRIG_PIN);
  // set sensor echo pin as input, enable pull-up
  DIST_REG &= ~(1<<ECHO_PIN); DIST_PORT |= (1<<ECHO_PIN);
  // set sensor output pin as output
  DIST_REG |= (1<<DIST_TRIGGER); DIST_PORT &= ~(1<<DIST_TRIGGER);
}

// Triggers a new measurement
void TriggerPing() {
    DIST_PORT |= (1<<TRIG_PIN);   // set trigger pin high
    _delay_us(15);
    DIST_PORT &= ~(1<<TRIG_PIN);  // set trigger pin low
}

// Returns the distance in centimeters
unsigned short PingCM() {
  TriggerPing();
  return pulse/UNIT_CM;
}

// Returns the distance in inches
unsigned short PingIN() {
  TriggerPing();
  return pulse/UNIT_IN;
}



