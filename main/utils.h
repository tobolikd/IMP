#ifndef IMP_UTILS_H
#define IMP_UTILS_H

#include "i2c-utils.h"
#include "ssd1306.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

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
#define GRAPH_LOWER_BOUND_START_X 8
#define GRAPH_LOWER_BOUND_START_PAGE 7
#define GRAPH_UPPER_BOUND_START_X 0
#define GRAPH_UPPER_BOUND_START_PAGE 3

#define TEMPERATURE_START_PAGE 0
#define TEMPERATURE_START_X 64

#define MAX_GRAPH_DATA 80
typedef uint8_t graph_data[MAX_GRAPH_DATA];


/**
 * initialize ssd1306 display settings
 */
void ssd1306_turn_on(dev_conf_t device);

void write_to_buff(display_buff *buff, const uint8_t *data, uint32_t xPos,
                   uint32_t page, uint32_t width, uint32_t pageCount);
void set_pixel(display_buff *buff, uint8_t x, uint8_t y);

void set_temperature(display_buff *buff, uint8_t temperature);
void draw_graph(display_buff *buff, graph_data data);

void show_temperature_info(dev_conf_t display, dev_conf_t senzor);
void show_humidity_info(dev_conf_t display, dev_conf_t senzor);

#endif // IMP_UTILS_H
