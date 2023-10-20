#ifndef IMP_SHT31_H
#define IMP_SHT31_H

#include "i2c-utils.h"

// more options can be added from documentation (pg 10)
// here are just default options used in the project
#define SHT31_CLK_STRETCHING_DISABLED 0x24

#define SHT31_REPEATABILITY_LOW 0x16
#define SHT31_REPEATABILITY_MEDIUM 0x0b
#define SHT31_REPEATABILITY_HIGH 0x00
// wait times are maximum measurement duration times from docs
#define SHT31_REPEATABILITY_WAIT_TIME_MS 30

#define SHT31_DATA_LENGTH 6
#define SHT31_VALUE_LENGTH 2
#define SHT31_CRC_LEGTH 1
#define SHT31_TEMP_IDX 0
#define SHT31_TEMP_CRC_IDX 2
#define SHT31_HUMI_IDX 3
#define SHT31_HUMI_CRC_IDX 5

typedef struct {
    float temperature;
    float humidity;
} sht31_data_t;

/**
 * measure temperature and humidity from sht31
 */
sht31_data_t sht31_get_data(dev_conf_t device);

/**
 * software reset sht31
 */
void sht31_init(dev_conf_t device);

#endif // IMP_SHT31_H
