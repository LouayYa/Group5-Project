#ifndef DHT_H
#define DHT_H

#include <stdint.h>
#include "esp_err.h"

typedef struct {
    float temperature;
    float humidity;
} dht_reading_t;

void dht_init(void);
esp_err_t dht_read(int sensor_num, dht_reading_t *reading);

#endif
