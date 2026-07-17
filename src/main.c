/*
 * File:   main.c
 * Author: Terry
 *
 */

//Config bits (PIC16F1829) 
#pragma config FOSC  = INTOSC   // internal osc, no ext crystal needed
#pragma config WDTE  = OFF      // watchdog off (turn on for final build)
#pragma config PWRTE = ON       // power-up timer on
#pragma config MCLRE = ON       // MCLR pin enabled
#pragma config CP    = OFF      // code protection off
#pragma config CPD   = OFF      // data protection off
#pragma config BOREN = ON       // brown-out reset on
#pragma config CLKOUTEN = OFF   // CLKOUT disabled
#pragma config IESO  = OFF
#pragma config FCMEN = OFF
//================ CONFIGURATION - LED =================//
#pragma config WRT = OFF, PLLEN = OFF, STVREN = ON
#pragma config BORV = LO, LVP = ON
//================ CONFIGURATION - PWM =================//

#include <xc.h>
#include <stdint.h>
#include "angle_cmd.h"
#include "adc_v2.h"  // gives you light_reading_t, ADC_Init, adc_scan_average, adc_send_result
#include "led.h"    
#include "servo.h"

#include "oled.h"
#define _XTAL_FREQ 2000000   // 2 MHz

//================ PIN DEFINITIONS - PWM ================//
#define LDR_TL 0 // RA0/AN0
#define LDR_TR 1 // RA1/AN1
#define LDR_BL 2 // RA2/AN2
#define LDR_BR 3 // RA4/AN3 (RA3 has no Analog select bit)

void main(void) {
    OSCCON = 0x68;
    
    // pin directions ? master mask
    TRISAbits.TRISA0 = 1;   // RA0 analog input PR1
    TRISAbits.TRISA1 = 1;   // RA1 analog input PR2
    TRISAbits.TRISA2 = 1;   // RA2 analog input PR3
    TRISAbits.TRISA4 = 1;   // RA4 analog input PR4
    TRISAbits.TRISA5 = 1;   // RA5 analog input Voltage Divider
    TRISCbits.TRISC0 = 0;   // RC0 output Batt
    TRISCbits.TRISC1 = 0;   // RC1 output Batt
    TRISCbits.TRISC2 = 0;   // RC2 output Batt
    TRISCbits.TRISC5 = 0;   // RC5 output pan servo PWM
    TRISCbits.TRISC3 = 0;   // RC3 output tilt servo PWM
    TRISBbits.TRISB4 = 1;   // RB4 analog input - OLED IC2
    TRISBbits.TRISB5 = 1;   // RB5 analog input - battery voltage
    TRISBbits.TRISB6 = 1;   // RB6 analog input - OLED IC2
    TRISBbits.TRISB7 = 1;   // RB7 input Batt pushbutton
    ANSELBbits.ANSB5 = 1;  // RB5 as analog input
    ANSELAbits.ANSA0 = 1;   // RA0 analog
    ANSELAbits.ANSA1 = 1;   // RA1 analog
    ANSELAbits.ANSA2 = 1;   // RA2 analog
    ANSELAbits.ANSA4 = 1;   // RA4 analog
    TRISCbits.TRISC7 = 1;
    ANSELCbits.ANSC7 = 1;
    
    //define PR structure
    light_reading_t reading = {0};
    angle_cmd_t cmd = {0};
    //Initialize all components
    ADC_Init();
    PWM_Init();
    Battery_Init();
    goHome();
    OLED_Init();
    OLED_Splash();
    
    //run and iterate loop
    int i = 0;
    while(1) {
        //read PR values
        adc_scan_average(&reading);
        //send to PWM
        adc_send_result(&reading, &cmd);
        //Move servos
        servo_update(cmd.lux_max, cmd.pan_error, cmd.tilt_error, &cmd);
        //Update OLED angle display
        if(i==25)
            {
            OLED_Update(cmd.pan_angle, cmd.tilt_angle);
            i=0;
            }
            i++;
        //Polls button to see if pushed
        Battery_Check();
    }
}