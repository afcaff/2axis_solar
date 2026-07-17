/*
 * angle_cmd.h
 * EE2583 Final Project - Team #13
 *
 * This is a STUB so Garrett can compile and test his module independently
 * before the real file is finalized. 
 *
 * Every module that needs to share data should #include this file.
 * Do NOT define variables here -- only the struct typedef.
 */

#ifndef ANGLE_CMD_H
#define ANGLE_CMD_H

#include <stdint.h>

/*
 * angle_cmd_t
 * The main inter-module communication struct.
 */
typedef struct {
    /* --- Written by A2D module  --- */
    int16_t  pan_error;       /* right - left, signed. + = pan right, - = pan left */
    int16_t  tilt_error;      /* top - bottom, signed. + = tilt up,   - = tilt down */
    uint16_t lux_top_left;         /* raw averaged ADC counts per sensor */
    uint16_t lux_top_right;
    uint16_t lux_bot_left;
    uint16_t lux_bot_right;
    uint16_t lux_max;         /* brightest reading this scan */
    uint8_t  brightest_sensor;/* SENSOR_TOP/RIGHT/BOTTOM/LEFT */
    uint8_t  data_ready;      /* set to 1 by A2D, cleared to 0 by algorithm */

    /* --- Written by Algorithm module  --- */
    uint8_t  pan_angle;       /* degrees, 0..120 */
    uint8_t  tilt_angle;      /* degrees, 0..120 */
} angle_cmd_t;

#endif /* ANGLE_CMD_H */