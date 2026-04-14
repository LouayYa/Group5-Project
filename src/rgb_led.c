#include "rgb_led.h"
#include "pins.h"
#include "driver/ledc.h"

static void rgb_set(ledc_channel_t r_ch, ledc_channel_t g_ch, ledc_channel_t b_ch,
                    ledc_mode_t mode, uint8_t r, uint8_t g, uint8_t b) {
    ledc_set_duty(mode, r_ch, r);
    ledc_update_duty(mode, r_ch);
    ledc_set_duty(mode, g_ch, g);
    ledc_update_duty(mode, g_ch);
    ledc_set_duty(mode, b_ch, b);
    ledc_update_duty(mode, b_ch);
}

void rgb_led_init(void) {
    // Timer 0 (low-speed): RGB LEDs at 5 kHz, 8-bit
    ledc_timer_config_t rgb_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&rgb_timer);

    struct { gpio_num_t pin; ledc_channel_t ch; } channels[] = {
        { RGB1_R_PIN, RGB1_R_CHANNEL },
        { RGB1_G_PIN, RGB1_G_CHANNEL },
        { RGB1_B_PIN, RGB1_B_CHANNEL },
        { RGB2_R_PIN, RGB2_R_CHANNEL },
        { RGB2_G_PIN, RGB2_G_CHANNEL },
        { RGB2_B_PIN, RGB2_B_CHANNEL },
        { RGB3_R_PIN, RGB3_R_CHANNEL },
        { RGB3_G_PIN, RGB3_G_CHANNEL },
    };

    for (int i = 0; i < sizeof(channels) / sizeof(channels[0]); i++) {
        ledc_channel_config_t cfg = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = channels[i].ch,
            .timer_sel = LEDC_TIMER_0,
            .intr_type = LEDC_INTR_DISABLE,
            .gpio_num = channels[i].pin,
            .duty = 0,
            .hpoint = 0
        };
        ledc_channel_config(&cfg);
    }

    // RGB3 blue on high-speed timer 3 (ran out of low-speed channels)
    ledc_timer_config_t rgb3b_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_3,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&rgb3b_timer);

    ledc_channel_config_t rgb3b_cfg = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = RGB3_B_CHANNEL,
        .timer_sel = LEDC_TIMER_3,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = RGB3_B_PIN,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&rgb3b_cfg);
}

void rgb1_set(uint8_t r, uint8_t g, uint8_t b) {
    rgb_set(RGB1_R_CHANNEL, RGB1_G_CHANNEL, RGB1_B_CHANNEL,
            LEDC_LOW_SPEED_MODE, r, g, b);
}

void rgb2_set(uint8_t r, uint8_t g, uint8_t b) {
    rgb_set(RGB2_R_CHANNEL, RGB2_G_CHANNEL, RGB2_B_CHANNEL,
            LEDC_LOW_SPEED_MODE, r, g, b);
}

void rgb3_set(uint8_t r, uint8_t g, uint8_t b) {
    // R and G on low-speed mode
    ledc_set_duty(LEDC_LOW_SPEED_MODE, RGB3_R_CHANNEL, r);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, RGB3_R_CHANNEL);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, RGB3_G_CHANNEL, g);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, RGB3_G_CHANNEL);
    // B on high-speed mode (separate timer, ran out of low-speed channels)
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, RGB3_B_CHANNEL, b);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, RGB3_B_CHANNEL);
}
