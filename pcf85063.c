
#include "pcf85063.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_err.h"
#include "i2c_lock.h"


// static char * TAG = "PCF85063";

static i2c_master_dev_handle_t dev_handle;


void init_rtc(i2c_master_bus_handle_t bus_handle)
{
    // Add device to I2C bus.
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCF85063_ADDR,
        .scl_speed_hz = PCF_I2C_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));
}


uint8_t bcd_to_dec(uint8_t val)
{
    return (val >> 4) * 10 + (val & 0x0F);
}


uint8_t dec_to_bcd(uint8_t val)
{
    return ((val / 10) << 4) | (val % 10);
}


esp_err_t rtc_read_reg(uint8_t reg_addr, uint8_t * data, size_t len)
{
    uint8_t send_buf[1] = { reg_addr };

    lock_i2c();
    esp_err_t ret = i2c_master_transmit_receive(dev_handle, send_buf, sizeof(send_buf), data, len, pdMS_TO_TICKS(PCF_I2C_TIMEOUT_MS));
    unlock_i2c();

    return ret;
}


esp_err_t rtc_write(uint8_t * data, size_t len)
{
    lock_i2c();
    esp_err_t ret = i2c_master_transmit(dev_handle, data, len, pdMS_TO_TICKS(PCF_I2C_TIMEOUT_MS));
    unlock_i2c();
    return ret;
}


void rtc_get_time(char * out, size_t max_length)
{
    uint8_t data[7];

    ESP_ERROR_CHECK(rtc_read_reg(PCF_TIME_DATE_REG_BEGIN, data, 7));

    uint8_t seconds = bcd_to_dec(data[0] & 0x7F); // Masking out stop bit
    uint8_t minutes = bcd_to_dec(data[1]);
    uint8_t hours = bcd_to_dec(data[2]);
    uint8_t day = bcd_to_dec(data[3]);
    uint8_t month = bcd_to_dec(data[5] & 0x1F); // Masking out century bit
    uint8_t year = bcd_to_dec(data[6]);

    snprintf(out, max_length, "%02d:%02d:%02d %02d/%02d/%04d", hours, minutes, seconds, month, day, 2000 + year);
}


void rtc_get_hms(char * out, size_t max_length)
{
    uint8_t data[7];

    ESP_ERROR_CHECK(rtc_read_reg(PCF_TIME_DATE_REG_BEGIN, data, 7));

    uint8_t seconds = bcd_to_dec(data[0] & 0x7F); // Masking out stop bit
    uint8_t minutes = bcd_to_dec(data[1]);
    uint8_t hours = bcd_to_dec(data[2]);

    snprintf(out, max_length, "%02d:%02d:%02d", hours, minutes, seconds);
}


void rtc_get_mdy(char * out, size_t max_length)
{
    uint8_t data[7];

    ESP_ERROR_CHECK(rtc_read_reg(PCF_TIME_DATE_REG_BEGIN, data, 7));

    uint8_t day = bcd_to_dec(data[3]);
    uint8_t month = bcd_to_dec(data[5] & 0x1F); // Masking out century bit
    uint8_t year = bcd_to_dec(data[6]);

    snprintf(out, max_length, "%02d/%02d/%04d", month + 1, day, 2000 + year);
}


void rtc_get_time_raw(uint8_t * out)
{
    uint8_t data[7];

    ESP_ERROR_CHECK(rtc_read_reg(PCF_TIME_DATE_REG_BEGIN, data, 7));

    // h m s d m y

    out[0] = bcd_to_dec(data[2]);
    out[1] = bcd_to_dec(data[1]);
    out[2] = bcd_to_dec(data[0] & 0x7F); // Masking out stop bit
    out[3] = bcd_to_dec(data[3]);
    out[4] = bcd_to_dec(data[5] & 0x1F); // Masking out century bit
    out[5] = bcd_to_dec(data[6]);
}


void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t month, uint8_t year)
{
    uint8_t data[8] = {
        PCF_TIME_DATE_REG_BEGIN,
        dec_to_bcd(seconds),
        dec_to_bcd(minutes),
        dec_to_bcd(hours),
        dec_to_bcd(day),
        dec_to_bcd(0), // Weekday, not needed.
        dec_to_bcd(month),
        dec_to_bcd(year)
    };
    ESP_ERROR_CHECK(rtc_write(data, 8));
}