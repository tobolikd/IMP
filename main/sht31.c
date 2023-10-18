#include "sht31.h"
#include "esp_log.h"
#include "i2c-utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const char tag[] = "sht31";

#define CRC_POLYNOMIAL 0x131
#define CRC_INIT 0xff
#define MSB_BIT_MASK (1 << 7)
uint8_t crc_check(uint8_t *data) {
    uint8_t crc = CRC_INIT;

    for (uint8_t i = 0; i < SHT31_VALUE_LENGTH; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & MSB_BIT_MASK) {
                crc <<= 1;
                crc ^= CRC_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

sht31_data_t sht31_get_data(dev_conf_t device) {
    uint8_t command[] = {SHT31_CLK_STRETCHING_DISABLED,
                         SHT31_REPEATABILITY_MEDIUM};

    i2c_send_data(device, command, sizeof(command));

    // wait for data
    vTaskDelay(MILI_SECONDS(SHT31_REPEATABILITY_WAIT_TIME_MS));

    uint8_t data[SHT31_DATA_LENGTH] = {0};

    i2c_read_data(device, data, SHT31_DATA_LENGTH);

    float temperature_data =
        (float)((data[SHT31_TEMP_IDX] * 256) + data[SHT31_TEMP_IDX + 1]);
    float humidity_data =
        (float)((data[SHT31_HUMI_IDX] << 8) + data[SHT31_HUMI_IDX + 1]);

    sht31_data_t ret = {
        // conversion formulas from documentation (pg 13)
        .temperature = -45 + 175 * temperature_data / ((1 << 16) - 1),
        .humidity = 100 * humidity_data / ((1 << 16) - 1),
    };

    ESP_LOGI(tag, "temperature: %f, humidity: %f", ret.temperature, ret.humidity);
    if (data[SHT31_TEMP_CRC_IDX] != crc_check(&data[SHT31_TEMP_IDX]))
        ESP_LOGW(tag, "temperature is not correct");
    if (data[SHT31_HUMI_CRC_IDX] != crc_check(&data[SHT31_HUMI_IDX]))
        ESP_LOGW(tag, "humidity is not correct");

    return ret;
}

void sht31_init(dev_conf_t device) {
    // software init from docs (pg 12)
    uint8_t data[] = {0x30, 0xa2};
    i2c_send_data(device, data, sizeof(data));
}
