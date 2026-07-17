//adc.c
// EE2583 Final Project - Team #13
// Module: A2D + Photoresistors
// Author: Garrett
// Modified by: Terry Herres 04.20.26
// Updated: Quadrant corner layout (TOP-LEFT, TOP-RIGHT, BOT-LEFT, BOT-RIGHT)

#include <xc.h>
#include <stdint.h>
#include "angle_cmd.h"
//#include "adc.h"
#include "adc_v2.h"

#define _XTAL_FREQ 2000000

/* -----------------------------------------------------------------------
 * PRIVATE: ADC channel for each quadrant sensor
 * ----------------------------------------------------------------------- */
static const uint8_t _adc_ch[NUM_SENSORS] = {
    0,   /* SENSOR_TOP_LEFT  -- AN0 on RA0 */
    1,   /* SENSOR_TOP_RIGHT -- AN1 on RA1 */
    2,   /* SENSOR_BOT_LEFT  -- AN2 on RA2 */
    3    /* SENSOR_BOT_RIGHT -- AN3 on RA4 (not RA3!) */
};

/* -----------------------------------------------------------------------
 * PRIVATE: Rolling window state
 * ----------------------------------------------------------------------- */
static uint16_t _ma_buffer[NUM_SENSORS][WINDOW_SIZE];
static uint32_t _ma_sum[NUM_SENSORS];
static uint8_t  _ma_index;
static uint8_t  _ma_count;

/* -----------------------------------------------------------------------
 * PRIVATE: _adc_read_channel
 * Select an AN channel and return a single 10-bit conversion result.
 * ----------------------------------------------------------------------- */
static uint16_t _adc_read_channel(uint8_t channel)
{
    ADCON0 = (uint8_t)((ADCON0 & 0x83) | ((channel & 0x1F) << 2));
    __delay_us(5);
    GO_nDONE = 1;
    while (GO_nDONE);
    return ((uint16_t)(ADRESH) << 8) | ADRESL;
}

/* -----------------------------------------------------------------------
 * PUBLIC: ADC_Init
 * Call once in main() before the main loop.
 * ----------------------------------------------------------------------- */
void ADC_Init(void)
{
    /* ADCON1: right-justify, Fosc/16 (correct for 2MHz), VDD/VSS reference */
    ADCON1 = 0b10110000;

    /* ADCON0: start on AN0, ADC ON */
    ADCON0 = 0b00000001;

    /* Zero the rolling window */
    for (uint8_t ch = 0; ch < NUM_SENSORS; ch++) {
        _ma_sum[ch] = 0;
        for (uint8_t i = 0; i < WINDOW_SIZE; i++)
            _ma_buffer[ch][i] = 0;
    }
    _ma_index = 0;
    _ma_count = 0;
}

/* -----------------------------------------------------------------------
 * PUBLIC: adc_scan_average
 * Call once per main loop iteration.
 * Takes one new sample per quadrant sensor, updates the rolling window,
 * and returns the current moving average for all four corners in *out.
 *
 * Pan axis:  compare (top_right + bot_right) vs (top_left + bot_left)
 * Tilt axis: compare (top_left + top_right)  vs (bot_left + bot_right)
 *
 *  out -- pointer to a light_reading_t struct to fill
 * ----------------------------------------------------------------------- */
void adc_scan_average(light_reading_t *out)
{
    /* Sample all sensors and update rolling window */
    for (uint8_t ch = 0; ch < NUM_SENSORS; ch++)
    {
        uint16_t new_sample = _adc_read_channel(_adc_ch[ch]);

        _ma_sum[ch] -= _ma_buffer[ch][_ma_index];
        _ma_buffer[ch][_ma_index] = new_sample;
        _ma_sum[ch] += new_sample;
    }

    /* Advance write head -- bitwise AND wraps at WINDOW_SIZE (power of 2) */
    _ma_index = (_ma_index + 1) & (WINDOW_SIZE - 1);

    /* Warm-up guard */
    if (_ma_count < WINDOW_SIZE)
        _ma_count++;

    /* Compute moving averages */
    out->top_left  = (uint16_t)(_ma_sum[SENSOR_TOP_LEFT]  / _ma_count);
    out->top_right = (uint16_t)(_ma_sum[SENSOR_TOP_RIGHT] / _ma_count);
    out->bot_left  = (uint16_t)(_ma_sum[SENSOR_BOT_LEFT]  / _ma_count);
    out->bot_right = (uint16_t)(_ma_sum[SENSOR_BOT_RIGHT] / _ma_count);

    /* Find the brightest quadrant */
    out->max_value  = out->top_left; 
    out->max_sensor = SENSOR_TOP_LEFT;

    if (out->top_right > out->max_value) { out->max_value = out->top_right; out->max_sensor = SENSOR_TOP_RIGHT; }
    if (out->bot_left  > out->max_value) { out->max_value = out->bot_left;  out->max_sensor = SENSOR_BOT_LEFT;  }
    if (out->bot_right > out->max_value) { out->max_value = out->bot_right; out->max_sensor = SENSOR_BOT_RIGHT; }
}

/* -----------------------------------------------------------------------
 * PUBLIC: adc_send_result
 *
 * Translates light_reading_t into the shared angle_cmd_t for Rogelio.
 *
 * Quadrant error signals:
 *   pan_error  = (top_right + bot_right) - (top_left + bot_left)
 *   tilt_error = (top_left  + top_right) - (bot_left + bot_right)
 *
 * Range: -2046 to +2046 (two sensors summed before subtracting)
 *   Positive pan_error  = more light on right -> pan right
 *   Negative pan_error  = more light on left  -> pan left
 *   Positive tilt_error = more light on top   -> tilt up
 *   Negative tilt_error = more light on bottom-> tilt down
 *
 *  reading -- filled light_reading_t from adc_scan_average()
 *  cmd     -- pointer to shared angle_cmd_t
 * ----------------------------------------------------------------------- */
void adc_send_result(const light_reading_t *reading, angle_cmd_t *cmd)
{
    /* Quadrant differential error signals */
    int16_t right_sum = (int16_t)reading->top_right + (int16_t)reading->bot_right;
    int16_t left_sum  = (int16_t)reading->top_left  + (int16_t)reading->bot_left;
    int16_t top_sum   = (int16_t)reading->top_left  + (int16_t)reading->top_right;
    int16_t bot_sum   = (int16_t)reading->bot_left  + (int16_t)reading->bot_right;

    cmd->pan_error  = right_sum - left_sum;
    cmd->tilt_error = top_sum   - bot_sum;

    /* Raw per-quadrant averaged readings */
    cmd->lux_top_left     = reading->top_left;
    cmd->lux_top_right    = reading->top_right;
    cmd->lux_bot_left     = reading->bot_left;
    cmd->lux_bot_right    = reading->bot_right; //these are being stored nowhere and also I don't think we need them, right?
    cmd->lux_max          = reading->max_value;
    cmd->brightest_sensor = reading->max_sensor;

    /* Set data_ready LAST -- all fields valid before Rogelio reads */
    cmd->data_ready = 1;
}
