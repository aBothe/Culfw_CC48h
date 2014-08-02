#ifndef _RF_CUNOTTY_H
#define _RF_CUNOTTY_H

extern uint8_t cunotty_on;

void rf_cunotty_init(void);
void rf_cunotty_task(void);
void rf_cunotty_func(char *in);

void cunotty_emitBroadcast(char* str, uint8_t len);

#endif

