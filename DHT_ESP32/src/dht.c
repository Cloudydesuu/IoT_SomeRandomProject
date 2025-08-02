#include "dht.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"


static gpio_num_t dht_gpio;
#define DHT11_TIMEOUT_US 100

static int wait_for_level(bool level, int timeout_us) {
    int time = 0;
    while (gpio_get_level(dht_gpio) != level) {
        if (++time > timeout_us) return -1;
        ets_delay_us(1);
    }
    return time;
}

esp_err_t dht11_init(gpio_num_t gpio) {
    dht_gpio = gpio;
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_level(dht_gpio, 1);
    return ESP_OK;
}

esp_err_t dht11_read(dht11_data *data) {
    uint8_t bits[5] = {0};

    // Start signal sending
    gpio_set_level(dht_gpio, 0);
    vTaskDelay(pdMS_TO_TICKS(20)); // Atleast 18ms
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(30);

    // Read response from DHT11
    if (wait_for_level(0, 80) < 0) return ESP_ERR_TIMEOUT;
    if (wait_for_level(1, 80) < 0) return ESP_ERR_TIMEOUT;

    // Read 40 bits of data
    for (int i = 0; i < 40; i++) {
        if (wait_for_level(0, DHT11_TIMEOUT_US) < 0) return ESP_ERR_TIMEOUT;

        int duration = wait_for_level(1, DHT11_TIMEOUT_US);
        if (duration < 0) return ESP_ERR_TIMEOUT;

        ets_delay_us(40);
        int bit = gpio_get_level(dht_gpio);
        bits[i / 8] <<= 1;
        bits[i / 8] |= bit;
    }

    // Check checksum
    uint8_t checksum = bits[0] + bits[1] + bits[2] + bits[3];
    if (bits[4] != checksum) {
        ESP_LOGE("DHT11", "Checksum error");
        return ESP_ERR_INVALID_CRC;
    }

    data->humidity = bits[0];
    data->temperature = bits[2];
    return ESP_OK;
}
