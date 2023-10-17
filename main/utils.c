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

    memset(buff, 0x00, DISPLAY_BUFF_SIZE);
    // get info
    // update num
    // update graph
    // structs to buf
    // display
    //
    static int num = 0;
    num++;
    num %= 100;

    set_temperature(&buff, num);
    draw_graph(&buff, NULL);

    if (ssd1306_write_buffer(display, buff) != ESP_OK)
        ESP_LOGE(tag, "failed to write buffer");
}
void show_humidity_info(dev_conf_t display, dev_conf_t senzor) {}

void set_temperature(display_buff *buff, uint8_t temperature) {
    // no support for temp greater than 99
    if (temperature >= 100)
        ESP_LOGW(tag, "temperature greater than 100 °C, first digit clipped");

    uint32_t xPosStart = TEMPERATURE_START_X;
    // write decimal
    if (temperature >= 10) {
        write_to_buff(buff, num16x24[(temperature % 100) / 10], xPosStart,
                      TEMPERATURE_START_PAGE, GLYPH_16x24_WIDTH,
                      GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
    }

    // move start to next digit
    xPosStart += GLYPH_16x24_WIDTH;
    write_to_buff(buff, num16x24[temperature % 10], xPosStart,
                  TEMPERATURE_START_PAGE, GLYPH_16x24_WIDTH,
                  GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);

    xPosStart += GLYPH_16x24_WIDTH;
    write_to_buff(buff, degree, xPosStart, TEMPERATURE_START_PAGE,
                  GLYPH_DEGREE_WIDTH,
                  GLYPH_DEGREE_HEIGHT / DISPLAY_PAGE_HEIGHT);

    xPosStart += GLYPH_DEGREE_WIDTH;
    write_to_buff(buff, letter16x24_C, xPosStart, TEMPERATURE_START_PAGE,
                  GLYPH_16x24_WIDTH, GLYPH_16x24_HEIGHT / DISPLAY_PAGE_HEIGHT);
}
void draw_graph(display_buff *buff, graph_data data) {
    // verical axis
    for (int i = GRAPH_AXIS_VERTICAL_START_Y; i < GRAPH_AXIS_VERTICAL_END_Y;
         i++)
        set_pixel(buff, GRAPH_AXIS_VERTICAL_X, i);
    // horizontal axis
    for (int i = GRAPH_AXIS_HORIZONTAL_START_X; i < GRAPH_AXIS_HORIZONTAL_END_X;
         i++)
        set_pixel(buff, i, GRAPH_AXIS_HORIZONTAL_Y);

    // set lower bound
    write_to_buff(buff, num8x8[0], GRAPH_LOWER_BOUND_START_X,
                  GRAPH_LOWER_BOUND_START_PAGE, GLYPH_8x8_WIDTH,
                  GLYPH_8x8_HEIGHT / DISPLAY_PAGE_HEIGHT);
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
