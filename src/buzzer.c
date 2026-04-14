#include "buzzer.h"
#include "pins.h"
#include "driver/ledc.h"

void buzzer_init(void) {
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_2,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ch = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = BUZZER_CHANNEL,
        .timer_sel = LEDC_TIMER_2,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = BUZZER_PIN,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ch);
}

void buzzer_tone(uint32_t freq_hz) {
    if (freq_hz == 0) {
        buzzer_off();
        return;
    }
    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_2, freq_hz);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL, 127);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL);
}

void buzzer_off(void) {
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL, 0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, BUZZER_CHANNEL);
}
