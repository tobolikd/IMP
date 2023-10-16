#ifndef IMP_SSD1306_H
#define IMP_SSD1306_H

#include "esp_err.h"
#include "i2c-utils.h"
#include <stdint.h>

#define OLED_CONTROL_BYTE_LENGTH 1

// conrol bytes (specifies next operation)
#define OLED_CONTROL_COMMAND_BYTE 0x80
#define OLED_CONTROL_COMMAND_STREAM 0x00
#define OLED_CONTROL_DATA_BYTE 0xc0
#define OLED_CONTROL_DATA_STREAM 0x40

#define OLED_SET_ADDRESSING_MODE 0x20
#define OLED_ADDRESSING_MODE_HORIZONTAL 0xa0
#define OLED_ADDRESSING_MODE_VERTICAL 0xa1

// horizontal addressing
#define OLED_SET_COLUMN_ADDR 0x21 // followed by 2 addresses in format *xxxxxxx

#define OLED_TURN_OFF 0xae
#define OLED_TURN_ON 0xaf

#define OLED_SET_MUX_RATIO 0xa8
#define OLED_MUX_64 0x3f

#define OLED_RESET_DISPLAY_START_LINE 0x40

#define OLED_SET_DISPLAY_OFFSET 0xd3 // followed by offset in format **xxxxxx

#define OLED_RESET_SEGMENT_REMAP 0xa1

#define OLED_COM_SCAN_DIRECTION_DEFAULT 0xc0
#define OLED_COM_SCAN_DIRECTION_FLIP 0xc8

#define OLED_SET_COM_PINS_CONFIG 0xda
#define OLED_COM_PINS_CONFIG_DEFAULT 0x12

#define OLED_SET_CONTRAST 0x81
#define OLED_CONTRAST_DEFAULT 0x7f

#define OLED_NORMAL_DISPLAY 0xa6
#define OLED_INVERSE_DISPLAY 0xa7

#define OLED_ON_RAM 0xa4
#define OLED_ON_FULL 0xa5

#define OLED_SET_OSCILATOR_FREQ 0xd5
#define OLED_OSCILATOR_FREQ_DEFAULT 0x80

#define OLED_SET_CHARGE_PUMP 0x8d
#define OLED_CHARGE_PUMP_ENABLE 0x14


/**
 * display buffer type (for additional type checking)
 *
 * the size is fixed for the ssd1306
 * width: 128
 * height: 64 / 8 = 8
 */
typedef uint8_t display_buff[8][128];

#define DISPLAY_BUFF_SIZE 1024 // 16 * 8
#define DISPLAY_NUM_PAGES 8
#define DISPLAY_PAGE_WIDTH 128
#define DISPLAY_PAGE_HEIGHT 8

esp_err_t ssd1306_send_commands(dev_conf_t device, uint8_t *commands,
                                uint8_t len);
esp_err_t ssd1306_write_buffer(dev_conf_t device, display_buff buf);

#endif // IMP_SSD1306_H
