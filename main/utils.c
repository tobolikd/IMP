#include "utils.h"
#include "esp_err.h"
#include "esp_log.h"
#include "glyphs.h"
#include "i2c-utils.h"
#include "ssd1306.h"
#include <stdint.h>
#include <string.h>

static const char *tag = "utils (IMP)";

void ssd1306_turn_on(dev_conf_t device) {
    // software startup sequence from documentation (Application Note pg 5)
    uint8_t commands[] = {
        // clang-format off
        OLED_SET_MUX_RATIO,                 OLED_MUX_64,
        OLED_SET_DISPLAY_OFFSET,            0x00,
        OLED_RESET_DISPLAY_START_LINE,
        OLED_RESET_SEGMENT_REMAP,
        OLED_COM_SCAN_DIRECTION_FLIP,
        OLED_SET_COM_PINS_CONFIG,           OLED_COM_PINS_CONFIG_DEFAULT,
        OLED_SET_CONTRAST,                  OLED_CONTRAST_DEFAULT,
        OLED_ON_RAM,
        OLED_NORMAL_DISPLAY,
        OLED_SET_OSCILATOR_FREQ,            OLED_OSCILATOR_FREQ_DEFAULT,
        OLED_SET_CHARGE_PUMP,               OLED_CHARGE_PUMP_ENABLE,
        OLED_TURN_ON,

        // custom settings
        OLED_SET_ADDRESSING_MODE, OLED_ADDRESSING_MODE_HORIZONTAL,
        // clang-format on
    };

    ssd1306_send_commands(device, commands, sizeof(commands));
}

void show_temperature_info(dev_conf_t display, dev_conf_t senzor) {
    static display_buff buff;
    static graph_data_t graph = {
        .data = {0},
        .min = INT16_MAX, // needs to be overwritten by first value
        .max = INT16_MIN,
        .idx = 0,
        .full = false};
    memset(buff, 0x00, DISPLAY_BUFF_SIZE);
    // get info
    // TODO
    static int16_t temperature = 18;
    temperature++;
    if (temperature > 50)
        temperature = 2;

    // set temp value
    set_metric_value(&buff, temperature);

    // set temp unit
    uint8_t xPosStart = METRIC_UNIT_START_X;
    write_to_buff(&buff, degree, xPosStart, METRIC_UNIT_START_PAGE,
                  GLYPH_DEGREE_WIDTH,
                  GLYPH_DEGREE_HEIGHT / DISPLAY_PAGE_HEIGHT);
    xPosStart += GLYPH_DEGREE_WIDTH;
    write_to_buff(&buff, letter16x24_C, xPosStart, METRIC_UNIT_START_PAGE,
                  GLYPH_16x24_WIDTH, GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);

    // update graph
    add_graph_data(&graph, temperature);
    draw_graph(&buff, &graph);

    // display
    if (ssd1306_write_buffer(display, buff) != ESP_OK)
        ESP_LOGE(tag, "failed to write buffer");
}

void show_humidity_info(dev_conf_t display, dev_conf_t senzor) {
    static display_buff buff;
    static graph_data_t graph = {.data = {0},
                                 .min = INT16_MAX,
                                 .max = INT16_MIN,
                                 .idx = 0,
                                 .full = false};
    memset(buff, 0x00, DISPLAY_BUFF_SIZE);

    // get humidity
    // TODO

    // set humidity value
    set_metric_value(&buff, 105);

    // set humidity unit
    write_to_buff(&buff, symbol16x24_percent, METRIC_UNIT_START_X,
                  METRIC_UNIT_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);

    // update graph
    draw_graph(&buff, &graph);

    // display
    if (ssd1306_write_buffer(display, buff) != ESP_OK)
        ESP_LOGE(tag, "failed to write buffer");
}

void set_metric_value(display_buff *buff, uint32_t value) {
    uint32_t xPosStart = METRIC_VALUE_START_X;
    // no support for temp greater than 999
    if (value >= 1000)
        ESP_LOGW(tag, "temperature greater than 100 °C, first digit clipped");

    if (value >= 100) {
        write_to_buff(buff, num16x24[(value % 1000) / 100], xPosStart,
                      METRIC_VALUE_START_PAGE, GLYPH_16x24_WIDTH,
                      GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    }

    // move start to next digit
    xPosStart += GLYPH_16x24_WIDTH;
    if (value >= 10) {
        write_to_buff(buff, num16x24[(value % 100) / 10], xPosStart,
                      METRIC_VALUE_START_PAGE, GLYPH_16x24_WIDTH,
                      GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    }

    xPosStart += GLYPH_16x24_WIDTH;
    write_to_buff(buff, num16x24[value % 10], xPosStart,
                  METRIC_VALUE_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);

    xPosStart += GLYPH_16x24_WIDTH;
}

void add_graph_data(graph_data_t *data, int16_t newValue) {
    if (newValue > data->max)
        data->max = newValue;
    if (newValue < data->min)
        data->min = newValue;

    if (data->full) {
        memmove(&data->data[0], &data->data[1],
                (MAX_GRAPH_DATA - 1) * sizeof(int16_t));
        data->data[MAX_GRAPH_DATA - 1] = newValue;
        return;
    }

    data->data[data->idx++] = newValue;
    if (data->idx >= MAX_GRAPH_DATA - 1)
        data->full = true;
}

void draw_graph(display_buff *buff, graph_data_t *data) {
    // verical axis
    for (int i = GRAPH_AXIS_VERTICAL_START_Y; i < GRAPH_AXIS_VERTICAL_END_Y;
         i++)
        set_pixel(buff, GRAPH_AXIS_VERTICAL_X, i);
    // horizontal axis
    for (int i = GRAPH_AXIS_HORIZONTAL_START_X; i < GRAPH_AXIS_HORIZONTAL_END_X;
         i++)
        set_pixel(buff, i, GRAPH_AXIS_HORIZONTAL_Y);

    int16_t data_range = ((data->max - data->min) < GRAPH_DATA_MIN_RANGE)
                             ? GRAPH_DATA_MIN_RANGE
                             : data->max - data->min;

    int16_t lower_bound = data->min;
    int16_t upper_bound = data->min + data_range;

    // set lower bound
    write_to_buff(buff, num8x8[lower_bound % 10], GRAPH_LOWER_BOUND_START_X,
                  GRAPH_LOWER_BOUND_START_PAGE, GLYPH_8x8_WIDTH,
                  GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);

    // set upper bound
    write_to_buff(buff, num8x8[(upper_bound % 100) / 10],
                  GRAPH_UPPER_BOUND_START_X, GRAPH_UPPER_BOUND_START_PAGE,
                  GLYPH_8x8_WIDTH, GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);
    write_to_buff(buff, num8x8[upper_bound % 10],
                  GRAPH_UPPER_BOUND_START_X + GLYPH_8x8_WIDTH,
                  GRAPH_UPPER_BOUND_START_PAGE, GLYPH_8x8_WIDTH,
                  GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);

    for (uint8_t i = 0; i <= data->idx; i++) {
        set_pixel(buff, GRAPH_DATA_START_X + i,
                  GRAPH_DATA_START_Y - ((data->data[i] - lower_bound) *
                                        GRAPH_POINT_MAX_RANGE / data_range));
    }
}

void write_to_buff(display_buff *buff, const uint8_t *data, uint32_t xPos,
                   uint32_t page, uint32_t width, uint32_t pageCount) {
    for (uint32_t idx = 0; idx < width * pageCount; idx++) {
        (*buff)[page + (idx / width)][xPos + (idx % width)] = data[idx];
    }
}

void set_pixel(display_buff *buff, uint8_t x, uint8_t y) {
    (*buff)[y / DISPLAY_PAGE_HEIGHT][x] |= 1 << (y % DISPLAY_PAGE_HEIGHT);
}
void play_animation(dev_conf_t display) {
    display_buff buff;
    memset(buff, 0, DISPLAY_BUFF_SIZE);

    for (uint8_t i = 0; i < ANIMATION_FRAMES; i++) {
        write_to_buff(&buff, temperature_animation[i], ANIMATION_TEMP_START_X,
                      ANIMATION_START_PAGE, GLYPH_16x24_WIDTH,
                      GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
        write_to_buff(&buff, humidity_animation[i], ANIMATION_HUMI_START_X,
                      ANIMATION_START_PAGE, GLYPH_16x24_WIDTH,
                      GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
        ssd1306_write_buffer(display, buff);
        vTaskDelay(MILI_SECONDS(ANIMATION_FRAME_DELAY_MS));
    }
}
