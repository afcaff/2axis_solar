#ifndef LED_H
#define LED_H

#include <xc.h>
#include <stdint.h>

#define DATA    LATCbits.LATC0
#define SRCLK   LATCbits.LATC1
#define RCLK    LATCbits.LATC2
#define BUTTON  PORTBbits.RB7

void Battery_Init(void);
void Battery_Check(void);

#endif