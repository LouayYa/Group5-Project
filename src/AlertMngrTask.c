#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "buzzer.h"
#include "servo.h"
#include "rgb_led.h"
#include "structs.h"

static const char *TAG = "AlertMngr";

extern QueueHandle_t xAlertQueue;

static void set_zone_led(int zone_id, zone_level_t level) {
    int r = 0, g = 0;
    if      (level == ZONE_CRITICAL) { r = 100; g = 0;   }
    else if (level == ZONE_WARNING)  { r = 100; g = 50;  }  // orange
    else                             { r = 0;   g = 100; }

    switch (zone_id) {
        case 1: rgb1_set(r, g, 0); break;
        case 2: rgb3_set(r, g, 0); break;
        case 3: rgb2_set(r, g, 0); break;
        default: break;
    }
}

void vAlertMngrTask(void *pvParameters) {
    AlertMsg_t msg;
    // Track the last known level for each zone (index = zone_id - 1)
    zone_level_t zone_levels[3] = { ZONE_NORMAL, ZONE_NORMAL, ZONE_NORMAL };

    while (1) {
        xQueueReceive(xAlertQueue, &msg, portMAX_DELAY);

        int idx          = msg.zone_id - 1;
        zone_level_t lvl = (zone_level_t)msg.level;

        zone_levels[idx] = lvl;
        set_zone_led(msg.zone_id, lvl);

        // Find the highest active level across all zones
        zone_level_t worst       = ZONE_NORMAL;
        int          worst_zone  = -1;
        for (int i = 0; i < 3; i++) {
            if (zone_levels[i] > worst) {
                worst      = zone_levels[i];
                worst_zone = i + 1;
            }
        }

        if (worst == ZONE_NORMAL) {
            // All zones clear — silence buzzer immediately
            buzzer_off();
            ESP_LOGI(TAG, "All zones normal — buzzer off");
        } else {
            // Point servo at worst zone and sound buzzer
            buzzer_tone(262);
            switch (worst_zone) {
                case 1: servo_set_angle(0);   break;
                case 2: servo_set_angle(90);  break;
                case 3: servo_set_angle(180); break;
                default: break;
            }
            ESP_LOGI(TAG, "Worst zone: %d level: %d", worst_zone, (int)worst);
        }
    }
}
