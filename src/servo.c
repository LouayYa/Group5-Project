#include "servo.h"
#include "pins.h"
#include "driver/ledc.h"

#define SERVO_DUTY_RES  LEDC_TIMER_16_BIT
#define SERVO_FREQ_HZ   50

static uint32_t pulse_to_duty(uint32_t pulse_us) {
    uint32_t max_duty = (1 << SERVO_DUTY_RES) - 1;
    uint32_t period_us = 1000000 / SERVO_FREQ_HZ;
    return (pulse_us * max_duty) / period_us;
}

void servo_init(void) {
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .duty_resolution = SERVO_DUTY_RES,
        .freq_hz = SERVO_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ch = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = SERVO_CHANNEL,
        .timer_sel = LEDC_TIMER_1,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = SERVO_PIN,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ch);
}

void servo_set_angle(uint32_t angle_deg) {
    if (angle_deg > 180) angle_deg = 180;
    uint32_t pulse_us = 500 + (angle_deg * 2000 / 180);
    uint32_t duty = pulse_to_duty(pulse_us);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, SERVO_CHANNEL, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, SERVO_CHANNEL);
}
