#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_err.h"

#include "structs.h"
#include "SensorTask.h"
#include "dht.h"
#include "potentiometer.h"

static const char *TAG = "SensorTask";

extern ZoneStatus_t zones[3];
extern SemaphoreHandle_t xZoneMutex;
extern QueueHandle_t xAlertQueue;

static zone_level_t classify_zone(float temp, float humidity, int gas) {
    if (temp >= TEMP_CRIT_THRESHOLD || humidity >= HUM_CRIT_THRESHOLD || gas >= GAS_CRIT_THRESHOLD)
        return ZONE_CRITICAL;
    if (temp >= TEMP_WARN_THRESHOLD || humidity >= HUM_WARN_THRESHOLD || gas >= GAS_WARN_THRESHOLD)
        return ZONE_WARNING;
    return ZONE_NORMAL;
}

void vZoneSensorTask(void *pvParameters) {
    int zone_id = *((int *)pvParameters);
    int zone_idx = zone_id - 1;

    const TickType_t xDelay = pdMS_TO_TICKS(500);

    while (1) {
        dht_reading_t reading;
        esp_err_t err = dht_read(zone_id, &reading);

        float temp     = 0.0f;
        float humidity = 0.0f;

        if (err == ESP_OK) {
            temp     = reading.temperature;
            humidity = reading.humidity;
        } else {
            ESP_LOGW(TAG, "Zone %d DHT read failed: %s", zone_id, esp_err_to_name(err));
        }

        int gas = potentiometer_read(zone_id);

        zone_level_t new_level = classify_zone(temp, humidity, gas);
        zone_level_t prev_level;

        if (xSemaphoreTake(xZoneMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            prev_level               = zones[zone_idx].level;
            zones[zone_idx].zone_id      = zone_id;
            zones[zone_idx].temperature  = temp;
            zones[zone_idx].humidity     = humidity;
            zones[zone_idx].gas          = gas;
            zones[zone_idx].level        = new_level;
            xSemaphoreGive(xZoneMutex);
        } else {
            ESP_LOGE(TAG, "Zone %d failed to take mutex", zone_id);
            vTaskDelay(xDelay);
            continue;
        }

        if (new_level != prev_level || new_level != ZONE_NORMAL) {
            AlertMsg_t msg = {
                .zone_id = zone_id,
                .level   = (int)new_level
            };
            if (xQueueSend(xAlertQueue, &msg, pdMS_TO_TICKS(10)) != pdTRUE) {
                ESP_LOGW(TAG, "Zone %d alert queue full", zone_id);
            }
            ESP_LOGI(TAG, "Zone %d — T:%.1f H:%.1f Gas:%d Level:%d",
                     zone_id, temp, humidity, gas, (int)new_level);
        }

        vTaskDelay(xDelay);
    }
}
