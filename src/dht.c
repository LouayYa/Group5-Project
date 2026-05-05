#include "dht.h"
#include "pins.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

static gpio_num_t dht_pin_for(int sensor_num) {
    switch (sensor_num) {
        case 1: return DHT1_PIN;
        case 2: return DHT2_PIN;
        case 3: return DHT3_PIN;
        default: return GPIO_NUM_NC;
    }
}

static int wait_for_level(gpio_num_t pin, int level, int timeout_us) {
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(pin) != level) {
        if ((esp_timer_get_time() - start) > timeout_us) return -1;
    }
    return (int)(esp_timer_get_time() - start);
}

void dht_init(void) {
    gpio_num_t pins[] = { DHT1_PIN, DHT2_PIN, DHT3_PIN };
    for (int i = 0; i < 3; i++) {
        gpio_set_direction(pins[i], GPIO_MODE_INPUT); // set dht pins as input
        gpio_set_pull_mode(pins[i], GPIO_PULLUP_ONLY); 
        // enable pull-up resistors because dht pulls the signal low to communicate, so we want it to default high when idle
    }
}

esp_err_t dht_read(int sensor_num, dht_reading_t *reading) {
    gpio_num_t pin = dht_pin_for(sensor_num);
    if (pin == GPIO_NUM_NC || reading == NULL) return ESP_ERR_INVALID_ARG;

    uint8_t data[5] = {0};

    // Send start signal: pull low for 1ms, then release
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    esp_rom_delay_us(1000);
    gpio_set_level(pin, 1);
    esp_rom_delay_us(30);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    // Wait for DHT response (low ~80us, high ~80us)
    if (wait_for_level(pin, 0, 100) < 0) return ESP_ERR_TIMEOUT;
    if (wait_for_level(pin, 1, 100) < 0) return ESP_ERR_TIMEOUT;
    if (wait_for_level(pin, 0, 100) < 0) return ESP_ERR_TIMEOUT;

    // Read 40 bits, 16-bit humidity + 16-bit temperature + 8-bit checksum
    for (int i = 0; i < 40; i++) {
        if (wait_for_level(pin, 1, 100) < 0) return ESP_ERR_TIMEOUT;
        int duration = wait_for_level(pin, 0, 100);
        if (duration < 0) return ESP_ERR_TIMEOUT;
        // reads a '1' if the high signal lasted >40us, otherwise it's a '0'
        if (duration > 40) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    // Verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) return ESP_ERR_INVALID_CRC;

    // DHT22: data is 16-bit humidity + 16-bit temperature (x10)
    reading->humidity = ((data[0] << 8) | data[1]) * 0.1f;
    reading->temperature = (((data[2] & 0x7F) << 8) | data[3]) * 0.1f;
    if (data[2] & 0x80) reading->temperature = -reading->temperature;

    return ESP_OK;
}
