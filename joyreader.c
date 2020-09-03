/*
   joyreader.c

   Created: 04/08/2020 13:36:10
    Author: Luca Bartolomei <bartn8@hotmail.it>
*/

#include "joyreader.h"
#include <avr/io.h>

//Global variables:
//Report is just a redundant copy of buttons and matrix buttons internal state.

volatile static buttons _buttons;
volatile static matrix_buttons _matrix_buttons;
volatile static uint8_t _report[JOYREADER_BUTTONS+JOYREADER_MATRIX_BUTTONS];

void joyreader_resetIO(){
	DDRB  = 0x00;
	DDRC  = 0x00;
	DDRD  = 0x00;
	
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
}

void joyreader_initButtons() {
  //DDRB Mixed: ROW0-ROW3 OUT, BTN2 BTN3 IN
  DDRB |= 0x0F;
  //DDRC All Input: COL0-COL3, BTN0 BTN1
  DDRC |= 0x00;
  //DDRD Mixed: BTN4 IN, COL4 IN, ROW4 OUT
  DDRD |= 0B10000000;
    

  //Matrix buttons needs external pull-down resistors.
  //BTN0-BTN4 use intenal pull-ups.
  PORTB |= (1 << PB4) | (1 << PB5);
  PORTC |= (1 << PC4) | (1 << PC5);
  PORTD |= (1 << PD5);


  //Initial state of buttons and matrix buttons
  //None of matrix buttons has a NOT logic.
  //README: Some of digital buttons have a NOT logic.

  for (uint8_t i = 0; i < JOYREADER_MATRIX_BUTTONS; i++)
  {
	_matrix_buttons[i]._negate = 0;
	_matrix_buttons[i]._counter = 0;
    _matrix_buttons[i]._state = JOYREADER_BTN_UP;    
  }

  for (uint8_t i = 0; i < JOYREADER_BUTTONS; i++) {
    _buttons[i]._counter = 0;
    _buttons[i]._state = JOYREADER_BTN_UP;
  }
  
  //I/O button configuration.
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_0)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_0)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_0)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_0)]._row = 0;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_0)]._col = 0;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_1)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_1)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_1)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_1)]._row = 1;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_1)]._col = 0;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_2)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_2)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_2)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_2)]._row = 2;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_2)]._col = 0;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_3)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_3)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_3)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_3)]._row = 3;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_3)]._col = 0;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_4)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_4)]._port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_4)]._pin_port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_4)]._row = 7;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_4)]._col = 0;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_5)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_5)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_5)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_5)]._row = 0;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_5)]._col = 1;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_6)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_6)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_6)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_6)]._row = 1;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_6)]._col = 1;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_7)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_7)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_7)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_7)]._row = 2;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_7)]._col = 1;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_8)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_8)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_8)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_8)]._row = 3;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_8)]._col = 1;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_9)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_9)]._port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_9)]._pin_port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_9)]._row = 7;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_9)]._col = 1;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_10)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_10)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_10)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_10)]._row = 0;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_10)]._col = 2;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_11)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_11)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_11)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_11)]._row = 1;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_11)]._col = 2;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_12)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_12)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_12)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_12)]._row = 2;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_12)]._col = 2;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_13)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_13)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_13)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_13)]._row = 3;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_13)]._col = 2;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_14)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_14)]._port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_14)]._pin_port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_14)]._row = 7;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_14)]._col = 2;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_15)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_15)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_15)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_15)]._row = 0;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_15)]._col = 3;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_16)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_16)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_16)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_16)]._row = 1;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_16)]._col = 3;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_17)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_17)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_17)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_17)]._row = 2;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_17)]._col = 3;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_18)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_18)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_18)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_18)]._row = 3;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_18)]._col = 3;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_19)]._pin = &PINC;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_19)]._port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_19)]._pin_port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_19)]._row = 7;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_19)]._col = 3;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_20)]._pin = &PIND;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_20)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_20)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_20)]._row = 0;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_20)]._col = 6;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_21)]._pin = &PIND;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_21)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_21)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_21)]._row = 1;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_21)]._col = 6;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_22)]._pin = &PIND;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_22)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_22)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_22)]._row = 2;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_22)]._col = 6;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_23)]._pin = &PIND;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_23)]._port = &PORTB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_23)]._pin_port = &PINB;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_23)]._row = 3;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_23)]._col = 6;
  
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_24)]._pin = &PIND;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_24)]._port = &PORTD;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_24)]._pin_port = &PIND;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_24)]._row = 7;
  _matrix_buttons[JOYREADER_GET_INDEX_MATRIX_BUTTON(JOYREADER_P_MATRIX_24)]._col = 6;

  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_0)]._pin = &PINC;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_1)]._pin = &PINC;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_2)]._pin = &PINB;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_3)]._pin = &PINB;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_4)]._pin = &PIND;

  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_0)]._port = &PORTC;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_1)]._port = &PORTC;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_2)]._port = &PORTB;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_3)]._port = &PORTB;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_4)]._port = &PORTD;

  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_0)]._pos = 4;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_1)]._pos = 5;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_2)]._pos = 4;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_3)]._pos = 5;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_4)]._pos = 5;

  //README
  //BTN0-BTN1 have a NOT logic!
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_0)]._negate = 1;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_1)]._negate = 1;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_2)]._negate = 0;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_3)]._negate = 0;
  _buttons[JOYREADER_GET_INDEX_BUTTON(JOYREADER_P_DIGITAL_4)]._negate = 0;

}

static inline void joyreader_buildReport() {

  //Just copy internal state in _report

  for (uint8_t i = 0; i < JOYREADER_BUTTONS; i++) {
    _report[JOYREADER_GET_BUTTON(i)] = _buttons[i]._state;
  }

  for (uint8_t i = 0; i < JOYREADER_MATRIX_BUTTONS; i++) {
    _report[JOYREADER_GET_MATRIX_BUTTON(i)] = _matrix_buttons[i]._state;
  }

}

//Just a getter
uint8_t* joyreader_getReport() {
  joyreader_buildReport();
  return _report;
}

uint8_t joyreader_updateButtons() {
  //Flag used to tell that a button has changed.
  uint8_t foundUpdate = 0;

  for (uint8_t i = 0; i < JOYREADER_BUTTONS; i++) {
    //Read current pin state
    uint8_t pinState = (*(_buttons[i]._pin) & (1 << _buttons[i]._pos)) ? JOYREADER_BTN_UP : JOYREADER_BTN_DOWN;

    //NOT logic?
    if (_buttons[i]._negate) {
      pinState = pinState == JOYREADER_BTN_DOWN ? JOYREADER_BTN_UP : JOYREADER_BTN_DOWN;
    }

    //If we have a state change, we must increment the counter.
    //If the counter is bigger than threshold, we reached a state change and we have to update the struct.
    if (pinState != _buttons[i]._state) {
      if (_buttons[i]._counter > JOYREADER_COUNTER_THRESHOLD) {
        _buttons[i]._state = pinState;
        _buttons[i]._counter = 0;
        foundUpdate = 1;
      } else {
        _buttons[i]._counter++;
      }
    } else {
      _buttons[i]._counter = 0;
    }
  }
  
  for (uint8_t i = 0; i < JOYREADER_MATRIX_BUTTONS; i++) {
    //Row is activated.
    *(_matrix_buttons[i]._port)|=(1 << _matrix_buttons[i]._row);

    //Must wait that the row is really activated.
    while (!((*_matrix_buttons[i]._pin_port) & (1 << _matrix_buttons[i]._row)));

    //Read col pin state.
    uint8_t pinState = (*(_matrix_buttons[i]._pin) & (1 << _matrix_buttons[i]._col)) ? JOYREADER_BTN_DOWN : JOYREADER_BTN_UP;

    //Not Logic?
    if (_matrix_buttons[i]._negate) {
      pinState = pinState == JOYREADER_BTN_DOWN ? JOYREADER_BTN_UP : JOYREADER_BTN_DOWN;
    }

    //If we have a state change, we must increment the counter.
    //If the counter is bigger than threshold, we reached a state change and we have to update the struct.
    if (pinState != _matrix_buttons[i]._state) {
      if (_matrix_buttons[i]._counter > JOYREADER_COUNTER_THRESHOLD) {
        _matrix_buttons[i]._state = pinState;
        _matrix_buttons[i]._counter = 0;
        foundUpdate = 1;
      } else {
        _matrix_buttons[i]._counter++;
      }
    } else {
      _matrix_buttons[i]._counter = 0;
    }

    //Row is deactivaed.
    *(_matrix_buttons[i]._port)&=~(1 << _matrix_buttons[i]._row);

    //Must wait that the row is really deactivated.
    while ((*_matrix_buttons[i]._pin_port) & (1 << _matrix_buttons[i]._row));
  }

  return foundUpdate;
}
