#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Cabeçalhos do nosso projeto
#include "display.h"
#include "config.h"

/**
 * @brief Inicializa o objeto do display SSD1306.
 * A inicialização do hardware I2C é feita separadamente no main.
 */
void display_init(ssd1306_t *ssd) {
    // Inicializa o objeto ssd1306, associando-o ao barramento I2C correto
    ssd1306_init(ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, DISPLAY_I2C_ADDR, I2C_PORT);

    // Envia a sequência de comandos de configuração para o display
    ssd1306_config(ssd);

    // Limpa o buffer interno e atualiza a tela
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
    printf("Display inicializado.\n");
}

/**
 * @brief Exibe uma tela de boas-vindas no momento da inicialização.
 */
void display_startup_screen(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    const char *line1 = "Receptor LoRa";
    const char *line2 = "Atividade 18";
    
    // Centraliza o texto horizontalmente
    uint8_t center_x = ssd->width / 2;
    uint8_t pos_x1 = center_x - (strlen(line1) * 8) / 2;
    uint8_t pos_x2 = center_x - (strlen(line2) * 8) / 2;
    
    ssd1306_draw_string(ssd, line1, pos_x1, 16);
    ssd1306_draw_string(ssd, line2, pos_x2, 36);
    
    ssd1306_send_data(ssd);
    sleep_ms(2000); // Mostra a mensagem por 2 segundos
}

/**
 * @brief Atualiza o display com o estado e o ângulo do portão.
 */
void display_update_gate_status(ssd1306_t *ssd, const char* status_line, const char* angle_line) {
    ssd1306_fill(ssd, false);

    // Centraliza a primeira linha
    uint8_t len1 = strlen(status_line);
    uint8_t pos_x1 = (ssd->width - (len1 * 8)) / 2;
    ssd1306_draw_string(ssd, status_line, pos_x1, 16);

    // Centraliza a segunda linha
    uint8_t len2 = strlen(angle_line);
    uint8_t pos_x2 = (ssd->width - (len2 * 8)) / 2;
    ssd1306_draw_string(ssd, angle_line, pos_x2, 36);

    ssd1306_send_data(ssd);
}