#include "ssd1306.h"
#include "esp_err.h"
#include "i2c-utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const char *tag = "ssd1306";

esp_err_t ssd1306_send_commands(dev_conf_t device, uint8_t *commands,
                                uint8_t len) {
    uint8_t *data = calloc(len + 1, sizeof(uint8_t));
    if (data == NULL)
        return ESP_ERR_NO_MEM;

    // set control byte to command stream
    data[0] = OLED_CONTROL_COMMAND_STREAM;
    memcpy(data + 1, commands, len);

    esp_err_t ret = i2c_send_data(device, data, len + 1);

    free(data);
    return ret;
}

esp_err_t ssd1306_write_buffer(dev_conf_t device, display_buff buf) {
    uint8_t data[DISPLAY_BUFF_SIZE + 1] = {0};

    // set control byte to data stream
    data[0] = OLED_CONTROL_DATA_STREAM;
    memcpy(data + 1, buf, DISPLAY_BUFF_SIZE);

    esp_err_t ret = i2c_send_data(device, data, DISPLAY_BUFF_SIZE + 1);

    return ret;
}
