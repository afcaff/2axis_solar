#ifndef ADC_PHOTORESISTOR_H_v2
#define ADC_PHOTORESISTOR_H_v2

#include <xc.h>
#include <stdint.h>
#include "angle_cmd.h"

#define SENSOR_TOP_LEFT    0
#define SENSOR_TOP_RIGHT  1
#define SENSOR_BOT_LEFT 2
#define SENSOR_BOT_RIGHT   3
#define NUM_SENSORS   4
#define WINDOW_SIZE   16

typedef struct {
    uint16_t top_left;
    uint16_t top_right;
    uint16_t bot_left;
    uint16_t bot_right;
    uint16_t max_value;
    uint8_t  max_sensor;
} light_reading_t;

void ADC_Init(void);
void adc_scan_average(light_reading_t *out);
void adc_send_result(const light_reading_t *reading, angle_cmd_t *cmd);

#endif