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

extern SemaphoreHandle_t xServiceMutex;

void vButtonTask(void *pvParameters) {
    while (1)
    {
        if(button_is_pressed()){
            if(xSemaphoreTake(xServiceMutex, portMAX_DELAY) == pdTRUE){
                servo_set_angle(45);
                vTaskDelay(pdMS_TO_TICKS(500));
                xSemaphoreGive(xServiceMutex);
            }
        }
    }
    
    
}