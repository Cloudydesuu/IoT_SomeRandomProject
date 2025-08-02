#include "dht.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define DHT_PIN GPIO_NUM_25

void app_main(void) {
    dht11_data dht_data;

    // Initialize the DHT11 sensor
    dht11_init(DHT_PIN);

    while (1) {
        // Read sensor data
        if (dht11_read(&dht_data) == ESP_OK) {
            ESP_LOGI("DHT11", "Temperature: %d°C, Humidity: %d", dht_data.temperature, dht_data.humidity);
        } else {
            ESP_LOGW("DHT11", "Failed to read data");
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000)); //delay 5 secs
    }
}
