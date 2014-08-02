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
char cunotty_msgbuf[255];

void rf_cunotty_init(void) {
	
}

// Receive messages and pipe them to the usb connector
void rf_cunotty_task(void){
	if(!cunotty_on)
		return;
	
	uint8_t msgLen;
		
	  // see if a CRC OK pkt has been arrived
  if (bit_is_set( CC1100_IN_PORT, CC1100_IN_PIN )) {
    /*msgLen = cc1100_readReg( CC1100_RXFIFO ) & 0x7f; // read len
    if (msg ) {
      // Something went horribly wrong, out of sync?
      rf_asksin_reset_rx();
      return;
    }*/

    CC1100_ASSERT;
    cc1100_sendbyte( CC1100_READ_BURST | CC1100_RXFIFO );
    
	if(cc1100_sendbyte(0) != 0xfe){
		DC('e'); DC('1'); DNL();
		return;}
	if(cc1100_sendbyte(0) != 0xed){
		DC('e'); DC('2'); DNL();
		return;}
	if(cc1100_sendbyte(0) != 0xfa){
		DC('e'); DC('3'); DNL();
		return;}
	if(cc1100_sendbyte(0) != 0xce){
		DC('e'); DC('4'); DNL();
		return;}
	
	msgLen = cc1100_sendbyte(0);
	
    for (uint8_t i=0; i<msgLen; i++) {
         cunotty_msgbuf[i] = cc1100_sendbyte( 0 );
    }
    
    if(cc1100_sendbyte( 0 ) != 0){
		DC('e'); DC('5'); DNL();
		return;
	}
    
    CC1100_DEASSERT;

    do {
      ccStrobe(CC1100_SRX);
    } while (cc1100_readReg(CC1100_MARCSTATE) != MARCSTATE_RX);
	
	DC('m'); // message
	DC(msgLen);
	for(uint8_t i = 0; i<msgLen; i++)
		DC(cunotty_msgbuf[i]);
	DNL();
	
    /*last_enc = msg[1];
    msg[1] = (~msg[1]) ^ 0x89;
    
    for (l = 2; l < msg[0]; l++) {
         this_enc = msg[l];
         msg[l] = (last_enc + 0xdc) ^ msg[l];
         last_enc = this_enc;
    }
    
    msg[l] = msg[l] ^ msg[2];
    
    if (tx_report & REP_BINTIME) {
      
      DC('a');
      for (uint8_t i=0; i<=msg[0]; i++)
      DC( msg[i] );
         
    } else {
      DC('A');
      
      for (uint8_t i=0; i<=msg[0]; i++)
        DH2( msg[i] );
      
      if (tx_report & REP_RSSI)
        DH2(rssi);
      
      DNL();
    }*/
  }

  switch(cc1100_readReg( CC1100_MARCSTATE )) {
    case MARCSTATE_RXFIFO_OVERFLOW:
      ccStrobe( CC1100_SFRX  );
    case MARCSTATE_IDLE:
      ccStrobe( CC1100_SIDLE );
      ccStrobe( CC1100_SNOP  );
      ccStrobe( CC1100_SRX   );
      break;
  }

#ifdef HAS_CC1101_RX_PLL_LOCK_CHECK_TASK_WAIT
  cc1101_RX_check_PLL_wait_task();
#endif
}

void cunotty_emitBroadcast(char* str, uint8_t len){
	my_delay_ms(3); 
	// enable TX, wait for CCA
	  do {
		ccStrobe(CC1100_STX);
	  } while (cc1100_readReg(CC1100_MARCSTATE) != MARCSTATE_TX);

		my_delay_ms(10);

	  // send
	  CC1100_ASSERT;
	  cc1100_sendbyte(CC1100_WRITE_BURST | CC1100_TXFIFO);
	  
	  // send magic bytes 0xfeedface
	  cc1100_sendbyte(0xfe);
	  cc1100_sendbyte(0xed);
	  cc1100_sendbyte(0xfa);
	  cc1100_sendbyte(0xce);
	  
	  // length
	  cc1100_sendbyte(len);

	  // msg payload
	  for(uint8_t i = 0; i < len; i++) {
		cc1100_sendbyte(str[i]);
	  }
	  
	  cc1100_sendbyte(0);

	  CC1100_DEASSERT;

	  // wait for TX to finish
	  while(cc1100_readReg( CC1100_MARCSTATE ) == MARCSTATE_TX)
		;

	  if (cc1100_readReg( CC1100_MARCSTATE ) == MARCSTATE_TXFIFO_UNDERFLOW) {
		  ccStrobe( CC1100_SFTX  );
		  ccStrobe( CC1100_SIDLE );
		  ccStrobe( CC1100_SNOP  );
	  }
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