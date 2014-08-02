#include "board.h"
#define HAS_CUNOTTY
#ifdef HAS_CUNOTTY
#include <string.h>
#include <avr/pgmspace.h>
#include "cc1100.h"
#include "delay.h"
#include "rf_receive.h"
#include "display.h"
#include "cc1101_pllcheck.h"

#include "rf_cunotty.h"

uint8_t cunotty_on = 0;

void rf_cunotty_init(void) {}

void rf_cunotty_task(void){
	if(!cunotty_on)
		return;
	
	// Receive messages and pipe them to the usb connector
}

void cunotty_emitBroadcast(char* str, uint8_t len){
	// 
}

void rf_cunotty_func(char* in){
	switch(in[1])
	{
		case 's':
			// Broadcast a message; first payload byte is the message size, the rest contains the actual message
			cunotty_emitBroadcast(in+3, in[2]);
		break;
		case 't':
			// Toggle reception; New status is set in in[2]
			cunotty_on = in[2];
		break;
	}
}



#endif