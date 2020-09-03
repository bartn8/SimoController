/*
   joyreader.h

   Created: 04/08/2020 13:25:29
    Author: Luca Bartolomei <bartn8@hotmail.it>
*/


#ifndef JOYREADER_H_
#define JOYREADER_H_

/*
Board v1.0 have 5 digital buttons and 25 matrix buttons.
PC4 and PC5 (BTN0 and BTN1) can be used as analog pins.
*/

#define JOYREADER_BUTTONS 5
#define JOYREADER_MATRIX_BUTTONS 25

/*
We have a matrix of 5x5 elements:
  Only one row is activated at the same time and buttons are checked using cols pins.
*/

#define JOYREADER_COLS 5
#define JOYREADER_ROWS 5

/*
Definition of logical state of the button.
*/

#define JOYREADER_BTN_UP 0
#define JOYREADER_BTN_DOWN 1

/*
Anti-bouncing threshold:
  Button change state when the counter is bigger than the threshold.
*/

#define JOYREADER_COUNTER_THRESHOLD 32

//README: Digital buttons must be the first one in the definitions to use those macros.

#define JOYREADER_GET_INDEX_BUTTON(X) (X)
#define JOYREADER_GET_INDEX_MATRIX_BUTTON(X) (X-JOYREADER_BUTTONS)

#define JOYREADER_GET_BUTTON(I) (I)
#define JOYREADER_GET_MATRIX_BUTTON(I) (I+JOYREADER_BUTTONS)


//Pull-up buttons
#define JOYREADER_P_DIGITAL_0 0
#define JOYREADER_P_DIGITAL_1 1
#define JOYREADER_P_DIGITAL_2 2
#define JOYREADER_P_DIGITAL_3 3
#define JOYREADER_P_DIGITAL_4 4

//Matrix buttons
#define JOYREADER_P_MATRIX_0 5
#define JOYREADER_P_MATRIX_1 6
#define JOYREADER_P_MATRIX_2 7
#define JOYREADER_P_MATRIX_3 8
#define JOYREADER_P_MATRIX_4 9

#define JOYREADER_P_MATRIX_5 10
#define JOYREADER_P_MATRIX_6 11
#define JOYREADER_P_MATRIX_7 12
#define JOYREADER_P_MATRIX_8 13
#define JOYREADER_P_MATRIX_9 14

#define JOYREADER_P_MATRIX_10 15
#define JOYREADER_P_MATRIX_11 16
#define JOYREADER_P_MATRIX_12 17
#define JOYREADER_P_MATRIX_13 18
#define JOYREADER_P_MATRIX_14 19

#define JOYREADER_P_MATRIX_15 20
#define JOYREADER_P_MATRIX_16 21
#define JOYREADER_P_MATRIX_17 22
#define JOYREADER_P_MATRIX_18 23
#define JOYREADER_P_MATRIX_19 24

#define JOYREADER_P_MATRIX_20 25
#define JOYREADER_P_MATRIX_21 26
#define JOYREADER_P_MATRIX_22 27
#define JOYREADER_P_MATRIX_23 28
#define JOYREADER_P_MATRIX_24 29

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Matrix buttons and digital buttons have different struct:
  Matrix buttons have a struct with two pin: row and col.
  Row pin is used as Output and col pin is used as Input.
*/

typedef struct
{
  uint8_t _state;		//Button state
  uint8_t *_port;		//PORT register of row
  uint8_t *_pin;		//PIN register of col
  uint8_t *_pin_port;	//PIN register of row
  uint8_t _row;			//Row pin register position
  uint8_t _col;			//Col pin register position
  uint8_t _negate;		//Negate button state
  uint8_t _counter;		//Threshold counter
} matrix_button;

typedef struct
{
  uint8_t _state;		//Button state
  uint8_t *_port;		//PORT register
  uint8_t *_pin;		//PIN register
  uint8_t _pos;			//Pin register position
  uint8_t _negate;		//Negate button state
  uint8_t _counter;		//Threshold counter
} button;

typedef button buttons[JOYREADER_BUTTONS];
typedef matrix_button matrix_buttons[JOYREADER_MATRIX_BUTTONS];

/************************************************************************/
/* Function used to reset I/O Registers                                 */
/************************************************************************/
void joyreader_resetIO();

/************************************************************************/
/* Function used to initialize I/O Registers and button states          */
/************************************************************************/
void joyreader_initButtons();

/************************************************************************/
/* Function used to get a report of the current state of buttons        */
/************************************************************************/
uint8_t* joyreader_getReport();

/************************************************************************/
/* Function used to read button states                                  */
/************************************************************************/
uint8_t joyreader_updateButtons();

#ifdef __cplusplus
}
#endif

#endif /* JOYREADER_H_ */
