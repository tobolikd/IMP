#ifndef IMP_UTILS_H
#define IMP_UTILS_H

#include "i2c-utils.h"
#include "ssd1306.h"
#include <stdint.h>

// graphic utils
#define LINE_VERTICAL 0x80
#define LINE_VERTICAL_TOP_HALF 0x0f

#define LINE_HORIZONTAL_BOTTOM 0x80

#define GRAPH_AXIS_VERTICAL_X 16
#define GRAPH_AXIS_VERTICAL_START_Y 24
#define GRAPH_AXIS_VERTICAL_END_Y 60

#define GRAPH_AXIS_HORIZONTAL_Y 55
#define GRAPH_AXIS_HORIZONTAL_START_X 12
#define GRAPH_AXIS_HORIZONTAL_END_X 96

static const uint8_t num8_0[] = {0, 60, 66, 66, 60, 0};

/**
 * initialize ssd1306 display settings
 */
void ssd1306_turn_on(dev_conf_t device);

void set_pixel(display_buff *buff, uint8_t x, uint8_t y);

void show_temperature_info(dev_conf_t display, dev_conf_t senzor);
void show_humidity_info(dev_conf_t display, dev_conf_t senzor);

#endif // IMP_UTILS_H
