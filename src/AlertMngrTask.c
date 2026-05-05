#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "buzzer.h"
#include "servo.h"
#include "rgb_led.h"
#include "structs.h"
#include "button.h"

static const char *TAG = "AlertMngr";

extern QueueHandle_t xAlertQueue;
extern SemaphoreHandle_t xServiceMutex;
extern volatile system_state_t system_state;

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
    zone_level_t zone_levels[3] = { ZONE_NORMAL, ZONE_NORMAL, ZONE_NORMAL };

    while (1) {
        // --- Check for button press (ISR-driven) ---
        if (xSemaphoreTake(xButtonSemaphore, 0) == pdTRUE) {
            if (system_state == SYS_ALERT) {
                system_state = SYS_ACKNOWLEDGED;
                ESP_LOGI(TAG, "Button pressed: ALERT -> ACKNOWLEDGED");
            } else if (system_state == SYS_ACKNOWLEDGED) {
                system_state = SYS_NORMAL;
                ESP_LOGI(TAG, "Button pressed: ACKNOWLEDGED -> NORMAL");
            }
        }

        // --- Wait for zone update (short timeout so button is checked regularly) ---
        if (xQueueReceive(xAlertQueue, &msg, pdMS_TO_TICKS(50)) != pdTRUE) {
            continue;
        }

        int idx          = msg.zone_id - 1;
        zone_level_t lvl = (zone_level_t)msg.level;

        zone_levels[idx] = lvl;
        set_zone_led(msg.zone_id, lvl);

        zone_level_t worst      = ZONE_NORMAL;
        int          worst_zone = -1;

        if (xSemaphoreTake(xServiceMutex, portMAX_DELAY) == pdTRUE) {

            for (int i = 0; i < 3; i++) {
                if (zone_levels[i] > worst) {
                    worst      = zone_levels[i];
                    worst_zone = i + 1;
                }
            }

            if (worst == ZONE_NORMAL) {
                // All clear — reset state and silence
                system_state = SYS_NORMAL;
                buzzer_off();
                ESP_LOGI(TAG, "All zones normal — buzzer off");
            } else {
                // Active alert zone exists
                if (system_state == SYS_NORMAL) {
                    // New alert condition — escalate
                    system_state = SYS_ALERT;
                }

                if (system_state == SYS_ALERT) {
                    buzzer_tone(440);
                    switch (worst_zone) {
                        case 1: servo_set_angle(0);   break;
                        case 2: servo_set_angle(90);  break;
                        case 3: servo_set_angle(180); break;
                        default: break;
                    }
                    ESP_LOGI(TAG, "ALERT — worst zone: %d level: %d", worst_zone, (int)worst);
                } else {
                    // SYS_ACKNOWLEDGED: buzzer silenced, servo stays
                    buzzer_off();
                    ESP_LOGI(TAG, "ACKNOWLEDGED — worst zone: %d (buzzer silenced)", worst_zone);
                }
            }

            xSemaphoreGive(xServiceMutex);
        }
    }
}
