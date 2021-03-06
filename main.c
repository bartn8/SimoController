/*
 * SimoController.c
 *
 * Created: 15/03/2020 20:32:37
 * Author : Luca Bartolomei <bartn8@hotmail.it>
 */ 

#ifndef F_CPU
#define F_CPU 12000000UL // 12 MHz clock speed
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv/usbdrv.h"

#include "joyreader.h"


//https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/

// ************************
// *** USB HID ROUTINES ***
// ************************

//https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/

//100
const PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, 0x01,                    //     REPORT_ID (1)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x0b,                    //     USAGE_MAXIMUM (Button 11)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x0b,                    //     REPORT_COUNT (11)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0xc0,                          // END_COLLECTION
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, 0x02,                    //     REPORT_ID (2)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x0c,                    //     USAGE_MINIMUM (Button 12)
    0x29, 0x16,                    //     USAGE_MAXIMUM (Button 22)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x0b,                    //     REPORT_COUNT (11)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x33,                    //     USAGE (Rx)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
};

typedef struct
{
	uint8_t reportID;
	uint16_t buttons;
	int8_t x;
	int8_t y;
} gamepad_report_t;

static uint8_t* joyreader_report;

static uint8_t idle_rate = 500 / 4; // see HID1_11.pdf sect 7.2.4
static uint8_t protocol_version = 0; // see HID1_11.pdf sect 7.2.6

static gamepad_report_t gamepad_report_1;
static gamepad_report_t gamepad_report_2;

/************************************************************************/
/* Build a USB Report with two XY Axis and 22 buttons.                  */
/************************************************************************/
static void buildReport(void){
	//Axis reading from report.
	
	if(joyreader_report[JOYREADER_P_MATRIX_17] == JOYREADER_BTN_DOWN)
		gamepad_report_1.y = 127;
	else if(joyreader_report[JOYREADER_P_MATRIX_18] == JOYREADER_BTN_DOWN)
		gamepad_report_1.y = -127;
	else
		gamepad_report_1.y = 0;
	
	if(joyreader_report[JOYREADER_P_MATRIX_19] == JOYREADER_BTN_DOWN)
		gamepad_report_1.x = 127;
	else if(joyreader_report[JOYREADER_P_MATRIX_20] == JOYREADER_BTN_DOWN)
		gamepad_report_1.x = -127;
	else
		gamepad_report_1.x = 0;
	
	if(joyreader_report[JOYREADER_P_MATRIX_21] == JOYREADER_BTN_DOWN)
		gamepad_report_2.y = 127;
	else if(joyreader_report[JOYREADER_P_MATRIX_22] == JOYREADER_BTN_DOWN)
		gamepad_report_2.y = -127;
	else
		gamepad_report_2.y = 0;
	
	if(joyreader_report[JOYREADER_P_MATRIX_23] == JOYREADER_BTN_DOWN)
		gamepad_report_2.x = 127;
	else if(joyreader_report[JOYREADER_P_MATRIX_24] == JOYREADER_BTN_DOWN)
		gamepad_report_2.x = -127;
	else
		gamepad_report_2.x = 0;
	
	//Report ID setter
	gamepad_report_1.reportID = 1;
	gamepad_report_2.reportID = 2;
	
	//Button state reset.
	gamepad_report_1.buttons = 0x0000;
	gamepad_report_2.buttons = 0x0000;
	
	//Buttons reading from report.
	
	if(joyreader_report[JOYREADER_P_DIGITAL_0] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1;
	
	if(joyreader_report[JOYREADER_P_DIGITAL_1] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 1;
		
	if(joyreader_report[JOYREADER_P_MATRIX_0] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 2;
		
	if(joyreader_report[JOYREADER_P_MATRIX_1] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 3;
	
	if(joyreader_report[JOYREADER_P_MATRIX_2] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 4;
		
	if(joyreader_report[JOYREADER_P_MATRIX_3] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 5;
	
	if(joyreader_report[JOYREADER_P_MATRIX_4] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 6;
	
	if(joyreader_report[JOYREADER_P_MATRIX_5] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 7;
	
	if(joyreader_report[JOYREADER_P_MATRIX_6] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 8;
		
	if(joyreader_report[JOYREADER_P_MATRIX_7] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 9;
		
	if(joyreader_report[JOYREADER_P_MATRIX_8] == JOYREADER_BTN_DOWN)
		gamepad_report_1.buttons |= (uint16_t)1 << 10;
	
	if(joyreader_report[JOYREADER_P_DIGITAL_2] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1;
	
	if(joyreader_report[JOYREADER_P_DIGITAL_3] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 1;
		
	if(joyreader_report[JOYREADER_P_DIGITAL_4] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 2;
			
	if(joyreader_report[JOYREADER_P_MATRIX_9] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 3;
	
	if(joyreader_report[JOYREADER_P_MATRIX_10] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 4;
		
	if(joyreader_report[JOYREADER_P_MATRIX_11] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 5;
	
	if(joyreader_report[JOYREADER_P_MATRIX_12] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 6;
	
	if(joyreader_report[JOYREADER_P_MATRIX_13] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 7;
	
	if(joyreader_report[JOYREADER_P_MATRIX_14] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 8;
		
	if(joyreader_report[JOYREADER_P_MATRIX_15] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 9;
		
	if(joyreader_report[JOYREADER_P_MATRIX_16] == JOYREADER_BTN_DOWN)
		gamepad_report_2.buttons |= (uint16_t)1 << 10;
	
}

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	// see HID1_11.pdf sect 7.2 and http://vusb.wikidot.com/driver-api
	usbRequest_t *rq = (void *)data;

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_CLASS)
	return 0; // ignore request if it's not a class specific request

	// see HID1_11.pdf sect 7.2
	switch (rq->bRequest)
	{
		case USBRQ_HID_GET_IDLE:
		usbMsgPtr = (usbMsgPtr_t)&idle_rate; // send data starting from this byte
		return 1; // send 1 byte
		case USBRQ_HID_SET_IDLE:
		idle_rate = rq->wValue.bytes[1]; // read in idle rate
		return 0; // send nothing
		case USBRQ_HID_GET_PROTOCOL:
		usbMsgPtr = (usbMsgPtr_t)&protocol_version; // send data starting from this byte
		return 1; // send 1 byte
		case USBRQ_HID_SET_PROTOCOL:
		protocol_version = rq->wValue.bytes[1];
		return 0; // send nothing
		case USBRQ_HID_GET_REPORT:
		
			// check for report ID then send back report
			if (rq->wValue.bytes[0] == 1)
			{
				usbMsgPtr = (usbMsgPtr_t)&gamepad_report_1;
				return sizeof(gamepad_report_t);
			}
			else if (rq->wValue.bytes[0] == 2)
			{
				usbMsgPtr = (usbMsgPtr_t)&gamepad_report_2;
				return sizeof(gamepad_report_t);
			}
			else
			{
				return 0; // no such report, send nothing
			}
		
		case USBRQ_HID_SET_REPORT: // no "output" or "feature" implemented, so ignore
		return 0; // send nothing
		default: // do not understand data, ignore
		return 0; // send nothing
	}
}


// this function is used to guarantee that the data is sent to the computer once
void usbSendHidReport(uchar * data, uchar len)
{
	while(1)
	{
		if (usbInterruptIsReady())
		{
			//Watchdog reset is placed here to prevent idle state from USB interrput.
			wdt_reset();
			
			usbSetInterrupt(data, len);
			break;
		}
		
		usbPoll();
	}
}

int main(void) {	
	//joyreader_resetIO();
	
	//LED: Used only as ON state LED. If watchdog is trigged, the LED will shutdown for a while.
	DDRD |= (1<<PD3);
	PORTD ^= (1<<PD3);	//Toggle LED
	
	// timer is initialized, used to keep track of idle period
	//1024 Prescaler
	TCCR1B = _BV(CS12) | _BV(CS10); 
	
	joyreader_initButtons();
	
	//Initial Report
	uint8_t to_send = 1; // boolean, true for first time
	to_send |= joyreader_updateButtons(); 
 	joyreader_report = joyreader_getReport();
		 
 	buildReport();
		
	wdt_enable(WDTO_4S); // enable 1s watchdog timer

	usbInit();
	usbDeviceDisconnect(); // enforce re-enumeration
	
	for(uint8_t i = 0; i<250; i++) { // wait 500 ms
		wdt_reset(); // keep the watchdog happy
		_delay_ms(2);
	}
	
	usbDeviceConnect();
	
	sei(); // Enable interrupts after re-enumeration
		
	while(1) {
		usbPoll();
				
		/*
		 * this area is where you should set the movement
		 * and button values of the reports using the input
		 * method of your choice
		 *
		*/
 		
		to_send |= joyreader_updateButtons(); 
 		joyreader_report = joyreader_getReport();
 		buildReport();
		 
		if (to_send == 0 && (idle_rate != 0 && (TCNT1 > ((4 * (F_CPU / 1024000)) * idle_rate) || TCNT1 > 0x7FFF)))
		{// using idle rate
			 to_send = 1;
		}
		
		if (usbInterruptIsReady())
		{
			//Watchdog reset is placed here to prevent idle state from USB interrput.
			wdt_reset();
			
			if(to_send != 0){
				// send the data if needed
			
				usbSendHidReport((uchar *)&gamepad_report_1,  sizeof(gamepad_report_t));
				usbSendHidReport((uchar *)&gamepad_report_2,  sizeof(gamepad_report_t));
			
				TCNT1 = 0; // reset timer
			}
		}
					
		//Reset flag
		to_send = 0;
	}
	
	return 0;
}
