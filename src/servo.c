// Name: Rogelio Maltos
// Date: 04/09/2026
// Modified by: Terry Herres
// Date: 04/20/2026
// Solar Panel Servos
// ---------------------------------------------------------------
// Configuration bits
// ---------------------------------------------------------------

#include <xc.h>
#include "servo.h"
#include "angle_cmd.h"

#define _XTAL_FREQ 2000000


// Servo positioning
static int x_posi = home_x; // creates reference points so we may code an easier while loop
static int y_posi = home_y; //
static int atHome = 0; // prevents work through the night
// PWM Setup
// DC value = pulse_us / 32
// upper 8 bits in CCPRxL
// Lower 2 bits in DCxB of CCPxCON
// PWM set up for x_axis servo
static void setPWM1(int pulse_us){
    int dc_value = pulse_us / 32;
    CCPR1L = (uint8_t)(dc_value >> 2); // shifts upper 8 bits right
    CCP1CONbits.DC1B = dc_value & 0x03; // equivalent to (CCP1CON & 0b11001111) |
}
// PWM set up for y_axis servo
static void setPWM2(int pulse_us){
    int dc_value = pulse_us / 32;
    CCPR2L = (uint8_t)(dc_value >> 2);
    CCP2CONbits.DC2B = dc_value & 0x03;
}
/* Clamp to keep solar panels from going trying to go beyond physical limit, keeps from high
current, drawing heat, and striping gears*/
static int clamp(int val, int lo, int hi){
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}
/* Drives both servos to home position doing top servo first to keep from traversing in awkward
angles*/
void goHome(){
    y_posi = home_y;
    setPWM2(y_posi);

    __delay_ms(500);
    x_posi = home_x;
    setPWM1(x_posi);
    __delay_ms(500);
    atHome = 1;
}

void PWM_Init(void){
    
    // Load PRx register with PWM Period
    PR2 = PR2_value;
    /* Configure CCP module for PWM mode by loading CCPxCON register with appropriate
    values*/
    CCPTMRS = 0;
    CCP1CON = 0x0C;
    setPWM1(home_x);
    CCP2CON = 0x0C;
    setPWM2(home_y);
    // Configure and start timer2/4/6
    T2CONbits.T2CKPS = 0b11;
    T2CONbits.TMR2ON = 1; 
    
}

// main
void servo_update(int16_t lux_max, int16_t x_err, int16_t y_err, angle_cmd_t *cmd){ //Need to make sure these are all ints, and also they output from pr in the right order.
//night detection
// all four ldrs below night thresh go home
    
    /*if (lux_max < Night_Threshold) // all sensors dark if max is below threshold 
    {
        if (!atHome) goHome();
        cmd->pan_angle  = 60;  
        cmd->tilt_angle = 60;
        __delay_ms(500);
        return;
    }*/
    // daytime clear home flag
    atHome = 0;

    // Dead Band and Step
    if (x_err > Dead_Band) x_posi = clamp(x_posi + step_size_x, Neg_x, Pos_x);
    if (x_err < -Dead_Band) x_posi = clamp(x_posi - step_size_x, Neg_x, Pos_x);
    if (y_err > Dead_Band) y_posi = clamp(y_posi + step_size_y, Neg_y, Pos_y);
    if (y_err < -Dead_Band) y_posi = clamp(y_posi - step_size_y, Neg_y, Pos_y);
    cmd->pan_angle  = (uint8_t)((x_posi - Neg_x) * 120 / (Pos_x - Neg_x));
    cmd->tilt_angle = (uint8_t)((y_posi - Neg_y) * 120 / (Pos_y - Neg_y));
    
    // update servos simultaneously
    
    
    setPWM1(x_posi);
    setPWM2(y_posi);
    __delay_ms(100);
}