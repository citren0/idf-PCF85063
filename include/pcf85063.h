
#ifndef PCF85063_H_
#define PCF85063_H_

#include "driver/gpio.h"
#include "driver/i2c_master.h"

#define PCF_I2C_SCL                 GPIO_NUM_10
#define PCF_I2C_SDA                 GPIO_NUM_11
#define PCF_I2C_NUM                 I2C_NUM_0
#define PCF_I2C_FREQ_HZ             200000
#define PCF_I2C_BUF_DISABLE         0
#define PCF_I2C_TIMEOUT_MS          1000
#define PCF85063_ADDR               0x51

#define PCF_TIME_DATE_REG_BEGIN     0x04

void init_rtc(i2c_master_bus_handle_t bus_handle);
void rtc_get_time(char * out, size_t max_length);
void rtc_get_hms(char * out, size_t max_length);
void rtc_get_mdy(char * out, size_t max_length);
void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month, uint8_t year);
void rtc_get_time_raw(uint8_t * out);

#endif // PCF85063_H_