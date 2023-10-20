#ifndef IMP_UTILS_H
#define IMP_UTILS_H

#include "i2c-utils.h"
#include "ssd1306.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// graph position
#define GRAPH_AXIS_VERTICAL_X 16
#define GRAPH_AXIS_VERTICAL_START_Y 24
#define GRAPH_AXIS_VERTICAL_END_Y 60

#define GRAPH_AXIS_HORIZONTAL_Y 55
#define GRAPH_AXIS_HORIZONTAL_START_X 12
#define GRAPH_AXIS_HORIZONTAL_END_X 96

#define GRAPH_LOWER_BOUND_START_X 0
#define GRAPH_LOWER_BOUND_START_PAGE 7
#define GRAPH_UPPER_BOUND_START_X 0
#define GRAPH_UPPER_BOUND_START_PAGE 3

// icon position
#define ICON_START_X 16
#define ICON_START_PAGE 0

// metric position
#define METRIC_VALUE_START_PAGE 0
#define METRIC_VALUE_END_X 96
#define METRIC_UNIT_START_PAGE 0
#define METRIC_UNIT_START_X 96
#define METRIC_DOT_START_X 79
#define METRIC_DOT_START_PAGE 2
// maximum metric glyphs (minus, whole part and fraction glyps in total)AGE 2
#define METRIC_MAX_DATA 4

// graph data position
#define GRAPH_DATA_START_X 17
#define GRAPH_DATA_START_Y 55
#define MAX_GRAPH_DATA 80
#define GRAPH_DATA_MIN_RANGE 2
#define GRAPH_POINT_MAX_RANGE 30
typedef struct GraphData {
    float data[MAX_GRAPH_DATA];
    float min;
    float max;
    uint32_t idx;
    bool full;
} graph_data_t;

/**
 * add graph data to graph data struct
 *
 * update minimum and maximum values
 */
void add_graph_data(graph_data_t *data, float newValue);

/**
 * initialize ssd1306 display settings and turn display on
 */
void ssd1306_turn_on(dev_conf_t device);

/**
 * writes rectangle area data to display buffer from the starting x position
 * and page
 *
 * xPos - pixel coord where the data should be written
 * page - page of the buffer to start writing
 * width - width of data on the screen
 * pageCount - height of data in pages
 */
void write_to_buff(display_buff *buff, const uint8_t *data, uint32_t xPos,
                   uint32_t page, uint32_t width, uint32_t pageCount);

/**
 * sets display pixel to high
 */
void set_pixel(display_buff *buff, uint8_t x, uint8_t y);

/**
 * sets metric numerical value to display buffer
 * removes leading zeros
 *
 * writes METRIC_MAX_DATA - 1 digits of the whole part or minus sign and
 * METRIC_MAX_DATA - 2 digits of the whole part
 *
 * fraction is limited to one digit
 */
void set_metric_value(display_buff *buff, int16_t whole_part, uint8_t fraction);

/**
 * sets graph data to buffer
 *
 * graph has horizontal and vertical axis
 *
 * vertical axis has integer lower and upper bounds which are minimum and
 * maximum values in graph data floored and ceiled respectively
 *
 * maps the graph data between the 2 bounds
 *
 * NOTE: bounds dont have support for negative values so eg. -15 is displayed as
 * 15, but the graph data are shown correctly
 */
void draw_graph(display_buff *buff, graph_data_t *data);

/**
 * plays simple startup animation with temperature and humidity icons with "IMP"
 * text in the middle
 */
void play_animation(dev_conf_t display);

/**
 * measures temperature, updates data in graph and displays everything on screen
 */
void show_temperature_info(dev_conf_t display, dev_conf_t sensor);

/**
 * measures humidity, updates data in graph and displays everything on screen
 */
void show_humidity_info(dev_conf_t display, dev_conf_t sensor);

#endif // IMP_UTILS_H
