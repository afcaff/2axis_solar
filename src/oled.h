#ifndef oled_h
#define oled_h

#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 2000000

void OLED_Init(void);
void OLED_Update(uint8_t pan, uint8_t tilt);
void OLED_Splash(void);

#endif