#ifndef SERVO_H
#define SERVO_H

#include <xc.h>
#include <stdint.h>

#define PR2_value    155
#define Neg_x        550
#define Pos_x        2550
#define home_x       1550
#define step_size_x  32
#define Neg_y        1050
#define Pos_y        2050
#define home_y       1550
#define step_size_y  32

//These two values may be the most likely to need modifications
#define Dead_Band        150
#define Night_Threshold  50


#include "angle_cmd.h"
void PWM_Init(void);
void goHome(void);
void servo_update(int16_t, int16_t, int16_t, angle_cmd_t *cmd);

#endif