#ifndef DHT_H
#define DHT_H

// This library provides functions to initialize and read data from multiple DHT sensors in ESP-IDF.
#include "driver/gpio.h"
#include "esp_err.h"

typedef struct {
    int temperature;
    int humidity;
} dht11_data;
esp_err_t dht11_init(gpio_num_t gpio);
esp_err_t dht11_read(dht11_data *data);


#endif
