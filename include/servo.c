#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo.h"

static uint slice_num;
static uint chan_num;

// Mapeia um valor de uma faixa para outra
static long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void servo_init(uint8_t servo_gpio) {
    gpio_set_function(servo_gpio, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(servo_gpio);
    chan_num = pwm_gpio_to_channel(servo_gpio);

    // Configura o PWM para 50Hz
    pwm_config config = pwm_get_default_config();
    // Clock do sistema (geralmente 125MHz) / (50Hz * 65535) ~= 38
    pwm_config_set_clkdiv(&config, 38.0f);
    pwm_init(slice_num, &config, true);
}

void servo_set_angle(uint8_t angle) {
    if (angle > 180) angle = 180;
    // Mapeia o ângulo (0-180) para o valor do duty cycle (aprox. 1638-8192)
    // Esses valores correspondem a pulsos de 0.5ms a 2.5ms a 50Hz.
    uint16_t duty = map(angle, 0, 180, 1638, 8192);
    pwm_set_chan_level(slice_num, chan_num, duty);
}