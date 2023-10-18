#include "utils.h"
#include "esp_err.h"
#include "esp_log.h"
#include "glyphs.h"
#include "i2c-utils.h"
#include "sht31.h"
#include "ssd1306.h"
#include <float.h>
#include <math.h>
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

void show_temperature_info(dev_conf_t display, dev_conf_t sensor) {
    static display_buff buff;
    static graph_data_t graph = {
        .data = {0},
        .min = FLT_MAX, // needs to be overwritten by first value
        .max = FLT_MIN,
        .idx = 0,
        .full = false};
    memset(buff, 0x00, DISPLAY_BUFF_SIZE);
    // get info
    float temperature = sht31_get_data(sensor).temperature;
    uint8_t fraction = (uint8_t)(fabs(temperature - (int16_t)temperature) * 10);

    // set temp value
    set_metric_value(&buff, (int16_t)temperature, fraction);

    // set temp unit
    uint8_t xPosStart = METRIC_UNIT_START_X;
    write_to_buff(&buff, degree, xPosStart, METRIC_UNIT_START_PAGE,
                  GLYPH_DEGREE_WIDTH,
                  GLYPH_DEGREE_HEIGHT / DISPLAY_PAGE_HEIGHT);
    xPosStart += GLYPH_DEGREE_WIDTH;
    write_to_buff(&buff, letter16x24_C, xPosStart, METRIC_UNIT_START_PAGE,
                  GLYPH_16x24_WIDTH, GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    // set icon
    write_to_buff(&buff, temperature_animation[0], ICON_START_X,
                  ICON_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);

    // update graph
    add_graph_data(&graph, temperature);
    draw_graph(&buff, &graph);

    // display
    if (ssd1306_write_buffer(display, buff) != ESP_OK)
        ESP_LOGE(tag, "failed to write buffer");
}

void show_humidity_info(dev_conf_t display, dev_conf_t sensor) {
    static display_buff buff;
    static graph_data_t graph = {
        .data = {0}, .min = FLT_MAX, .max = FLT_MIN, .idx = 0, .full = false};
    memset(buff, 0x00, DISPLAY_BUFF_SIZE);

    // get humidity
    float humidity = sht31_get_data(sensor).humidity;
    uint8_t fraction = (uint8_t)(fabs(humidity - (int16_t)humidity) * 10);

    // set humidity value
    set_metric_value(&buff, (int16_t)humidity, fraction);

    // set humidity unit
    write_to_buff(&buff, symbol16x24_percent, METRIC_UNIT_START_X,
                  METRIC_UNIT_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    // set icon
    write_to_buff(&buff, humidity_animation[0], ICON_START_X, ICON_START_PAGE,
                  GLYPH_16x24_WIDTH, GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);

    // update graph
    add_graph_data(&graph, humidity);
    draw_graph(&buff, &graph);

    // display
    if (ssd1306_write_buffer(display, buff) != ESP_OK)
        ESP_LOGE(tag, "failed to write buffer");
}

void set_metric_value(display_buff *buff, int16_t whole_part,
                      uint8_t fraction) {
    bool negative = whole_part < 0;
    if (negative)
        whole_part = -whole_part;

    uint8_t data_len = 0;
    // pointers to glyph data
    uint8_t const *data[METRIC_MAX_DATA] = {NULL};

    // set fraction part
    data[0] = num16x24[fraction];
    data_len++;

    // parse value digit by digit
    uint32_t digit_modulo = 10;
    for (uint8_t i = data_len; i < METRIC_MAX_DATA; i++) {
        // set current digit
        data[i] = num16x24[whole_part % (digit_modulo)];

        whole_part /= digit_modulo;
        data_len++;
        // first digit is always processed even if its 0
        // other leading zeros get cut off
        if (whole_part == 0)
            break;
    }

    if (whole_part != 0)
        ESP_LOGW(tag, "metric value display size exceeded space on screen");

    // set minus sign
    if (negative) {
        if (data_len == METRIC_MAX_DATA) {
            data_len--;
            ESP_LOGW(tag, "metric value display size exceeded space on screen");
        }
        data[data_len] = symbol16x24_minus;
        data_len++;
    }

    // display values
    uint32_t xPosStart = METRIC_VALUE_END_X;
    for (uint8_t i = 0; i < data_len; i++) {
        xPosStart -= GLYPH_16x24_WIDTH;
        if (data[i] == NULL)
            break;
        write_to_buff(buff, data[i], xPosStart, METRIC_VALUE_START_PAGE,
                      GLYPH_16x24_WIDTH,
                      GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    }
    // set dot
    write_to_buff(buff, symbol2x8_dot, METRIC_DOT_START_X,
                  METRIC_DOT_START_PAGE, GLYPH2x8_WIDTH,
                  GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);
}

void update_min_max(graph_data_t *data) {
    data->min = data->data[0];
    data->max = data->data[0];
    for (uint32_t i = 1; i < data->idx; i++) {
        if (data->data[i] < data->min)
            data->min = data->data[i];
        else if (data->data[i] > data->max)
            data->max = data->data[i];
    }
}

void add_graph_data(graph_data_t *data, float newValue) {
    if (data->full) {
        memmove(&data->data[0], &data->data[1],
                (MAX_GRAPH_DATA - 1) * sizeof(float));
        data->data[MAX_GRAPH_DATA - 1] = newValue;
        update_min_max(data);
        return;
    }
    if (newValue > data->max)
        data->max = newValue;
    if (newValue < data->min)
        data->min = newValue;

    data->data[data->idx++] = newValue;
    if (data->idx >= MAX_GRAPH_DATA)
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

    int16_t data_range = fabs(ceil(data->max) - floor(data->min));
    int16_t lower_bound;
    int16_t upper_bound;
    if (data_range < GRAPH_DATA_MIN_RANGE) {
        // if data are too close to each other center them
        float margin = (GRAPH_DATA_MIN_RANGE - data_range) / 2.;
        data_range = GRAPH_DATA_MIN_RANGE;
        lower_bound = data->min - margin;
        upper_bound = lower_bound + data_range;
    } else {
        lower_bound = floor(data->min);
        upper_bound = ceil(data->max);
    }

    // set lower bound (nos support for negative numbers)
    write_to_buff(buff, num8x8[(abs(lower_bound) % 100) / 10],
                  GRAPH_LOWER_BOUND_START_X, GRAPH_LOWER_BOUND_START_PAGE,
                  GLYPH_8x8_WIDTH, GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);
    write_to_buff(buff, num8x8[abs(lower_bound) % 10],
                  GRAPH_LOWER_BOUND_START_X + GLYPH_8x8_WIDTH,
                  GRAPH_LOWER_BOUND_START_PAGE, GLYPH_8x8_WIDTH,
                  GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);

    // set upper bound
    write_to_buff(buff, num8x8[(abs(upper_bound) % 100) / 10],
                  GRAPH_UPPER_BOUND_START_X, GRAPH_UPPER_BOUND_START_PAGE,
                  GLYPH_8x8_WIDTH, GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);
    write_to_buff(buff, num8x8[abs(upper_bound) % 10],
                  GRAPH_UPPER_BOUND_START_X + GLYPH_8x8_WIDTH,
                  GRAPH_UPPER_BOUND_START_PAGE, GLYPH_8x8_WIDTH,
                  GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);

    for (uint8_t i = 0; i < data->idx; i++) {
        // map pixels to point area
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

    // write text IMP
    uint8_t offset = 0;
    write_to_buff(&buff, letter16x24_I, ANIMATION_TEXT_IMP_START_X + offset,
                  ANIMATION_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    offset += GLYPH_16x24_WIDTH;
    write_to_buff(&buff, letter16x24_M, ANIMATION_TEXT_IMP_START_X + offset,
                  ANIMATION_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    offset += GLYPH_16x24_WIDTH;
    write_to_buff(&buff, letter16x24_P, ANIMATION_TEXT_IMP_START_X + offset,
                  ANIMATION_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);

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
