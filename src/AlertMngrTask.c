#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "buzzer.h"
#include "servo.h"
#include "rgb_led.h"


int AlertManager(QueueHandle_t alertQue, QueueHandle_t zoneQue){
    int level; 
    int current_level = 0;
    int id;
    //traks when we should kill the buzzer
    int hazardless_count = 0;
    while (1)
    {
        xQueueReceive(alertQue, &level, portMAX_DELAY);
        xQueueReceive(zoneQue, &id, portMAX_DELAY);
        if((level > current_level) && (level > 0)){
            hazardless_count = 0;
            current_level = level;
            ESP_LOG("hazard detected");
            //buzzer task here since that needs to be looped
            buzzer_tone(262);
            switch (id)
            {
                //this assunes id 1 = top zone, id 2 = mid zone, id 3 = bottom zone
            case 1:
                servo_set_angle(0);
                rgb1_set(100, 0, 0);
                break;
            
            case 2:
                servo_set_angle(90);
                rgb2_set(100, 0, 0);
                break;
            
            case 3:
                servo_set_angle(180);
                rgb3_set(100, 0, 0);
                break;

            default:
                break;
            }
        }
        else if(level = 0){
            hazardless_count++;
            switch (id)
            {
            case 1:
                rgb1_set(0, 100, 0);
                break;
            
            case 2:
                rgb2_set(0, 100, 0);
                break;
            
            case 3:
                rgb3_set(0, 100, 0);
                break;

            default:
                break;
            }

            if(hazardless_count >= 3){
                buzzer_off();
            }
        }
    }
    
    return 0;
}