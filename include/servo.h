#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

/**
 * @brief Inicializa o pino e o PWM para controlar o servo.
 * @param servo_gpio O pino GPIO conectado ao fio de sinal do servo.
 */
void servo_init(uint8_t servo_gpio);

/**
 * @brief Move o servo para um ângulo específico.
 * @param angle Ângulo desejado entre 0 e 180 graus.
 */
void servo_set_angle(uint8_t angle);

#endif