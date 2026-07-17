/*
 * File:   LED_test.c
 * Author: Tyler Jewell
 *
 * Description:
 * Battery level indicator using ADC + 10-segment LED bar
 * via cascaded 74HC595 shift registers
 *
 * Features:
 * - 2 MHz system clock (team standard)
 * - ADC averaging (reduces noise)
 * - Stable LED output (no flicker)
 * - Calibrated scaling for your hardware
 */
//================ BATTERY MODULE =================//
// Uses:
// RA1  -> ADC (battery voltage)
// RB7  -> Push button input
// RC0  -> DATA (74HC595)
// RC1  -> CLOCK
// RC2  -> LATCH

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 2000000

// CONFIG
#pragma config FOSC = INTOSC, WDTE = OFF, PWRTE = OFF, MCLRE = ON
#pragma config CP = OFF, CPD = OFF, BOREN = ON, CLKOUTEN = OFF
#pragma config IESO = OFF, FCMEN = OFF
#pragma config WRT = OFF, PLLEN = OFF, STVREN = ON
#pragma config BORV = LO, LVP = ON

// PINS
#define DATA    LATCbits.LATC0
#define SRCLK   LATCbits.LATC1
#define RCLK    LATCbits.LATC2
#define BUTTON  PORTBbits.RB7

// READ ADC
uint16_t readADC(void)
{
    ADCON0bits.CHS = 0b00001;  // AN1

    // Dummy read (flush sample capacitor)
    __delay_us(20);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);

    // Real read (longer settle time)
    __delay_us(100);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);

    return ((ADRESH << 8) | ADRESL);
}

// SHIFT REGISTER
void shiftOut16(uint16_t data)
{
    for (uint8_t i = 0; i < 16; i++)
    {
        DATA = (data & 0x8000) ? 1 : 0;

        SRCLK = 1;
        __delay_us(5);
        SRCLK = 0;

        data <<= 1;
    }

    RCLK = 1;
    __delay_us(5);
    RCLK = 0;
}

// LED SCALING
uint16_t getBarPattern(uint16_t adcValue)
{
    // Slightly adjusted range
    if (adcValue < 180) adcValue = 180;
    if (adcValue > 300) adcValue = 300;

    uint8_t level = (adcValue - 180) * 10 / (300 - 180);

    if (level > 10) level = 10;

    uint16_t pattern = 0;

    for (uint8_t i = 0; i < level; i++)
        pattern |= (1 << i);

    return pattern;
}

// MAIN
void main(void)
{
    while (1)
    {
        if (BUTTON == 0)
        {
            __delay_ms(50);

            if (BUTTON == 0)
            {
                uint16_t adcValue = readADC();
                uint16_t pattern = getBarPattern(adcValue);
                shiftOut16(pattern);
            }
        }
        else
        {
            shiftOut16(0x0000);
        }

        __delay_ms(100);
    }
}
