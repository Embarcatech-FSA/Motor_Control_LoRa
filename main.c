#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>

#include "include/config.h"
#include "include/lora.h"
#include "include/display.h"
#include "include/servo.h"   
#include "include/led_rgb.h" // Presumindo que você tenha um led_rgb.h

// Enum para os comandos recebidos
typedef enum { CMD_NONE, CMD_OPEN, CMD_STOP, CMD_CLOSE } Command;

// Enum para os estados do portão
typedef enum { STATE_CLOSED, STATE_OPENING, STATE_CLOSING, STATE_STOPPED, STATE_OPEN } GateState;

// Variáveis de controle globais
volatile Command received_command = CMD_NONE;
GateState current_state = STATE_CLOSED;
uint8_t servo_angle = 0;
uint32_t last_move_time_ms = 0;
ssd1306_t display;
// --- FUNÇÕES DE INICIALIZAÇÃO DE HARDWARE ---

/**
 * @brief Inicializa o barramento I2C1 para o display OLED.
 */
void setup_i2c_display() {
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    printf("I2C1 (Display) inicializado nos pinos SDA=%d, SCL=%d.\n", I2C_SDA_PIN, I2C_SCL_PIN);
}

/**
 * @brief Inicializa o barramento SPI0 e os pinos GPIO associados para o LoRa.
 * ESTA FUNÇÃO É A CORREÇÃO CRÍTICA.
 */
void setup_spi_lora() {
    // Inicializa o periférico SPI em si
    spi_init(LORA_SPI_PORT, 5 * 1000 * 1000); // 5 MHz

    // Informa aos pinos GPIO para serem controlados pelo periférico SPI
    gpio_set_function(LORA_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LORA_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LORA_MISO_PIN, GPIO_FUNC_SPI);
    
    printf("SPI0 (LoRa) inicializado nos pinos SCK=%d, MOSI=%d, MISO=%d.\n", LORA_SCK_PIN, LORA_MOSI_PIN, LORA_MISO_PIN);
}

/**
 * @brief Função de callback do LoRa. Coloca comandos recebidos na fila.
 */
void on_lora_receive(lora_payload_t* payload) {
    if (strncmp((const char*)payload->message, "CMD_OPEN", payload->length) == 0) {
        received_command = CMD_OPEN;
    } else if (strncmp((const char*)payload->message, "CMD_STOP", payload->length) == 0) {
        received_command = CMD_STOP;
    } else if (strncmp((const char*)payload->message, "CMD_CLOSE", payload->length) == 0) {
        received_command = CMD_CLOSE;
    }
}

int main() {
    stdio_init_all();
    sleep_ms(3000);
    printf("--- Iniciando Receptor de Controle de Servo ---\n");

    // ==========================================================
    // --- 1. INICIALIZAÇÃO CORRETA, seguindo o modelo que funciona ---
    // ==========================================================
    
    // a. Inicializa todos os periféricos de GPIO e I2C primeiro
    rgb_led_init();
    setup_i2c_display();
    setup_spi_lora();
    display_init(&display);
    servo_init(SERVO_PIN);
    
    // b. Mostra a tela de startup AGORA, pois o display já está pronto
    printf("Perifericos (LED, Display, Servo) OK. Mostrando tela de inicio...\n");
    display_startup_screen(&display);
    rgb_led_set_color(COR_LED_AMARELO); // Amarelo = inicializando

    
lora_config_t config = {
        .spi_port = LORA_SPI_PORT,
        .interrupt_pin = LORA_INTERRUPT_PIN,
        .cs_pin = LORA_CS_PIN,
        .reset_pin = LORA_RESET_PIN,
        .freq = LORA_FREQUENCY,
        .tx_power = LORA_TX_POWER,
        .this_address = LORA_ADDRESS_RECEIVER
    };

    if (!lora_init(&config)) {
        printf("ERRO FATAL: Falha na inicializacao do LoRa.\n");
        rgb_led_set_color(COR_LED_VERMELHO); 
        display_update_gate_status(&display, "ERRO FATAL:", "LoRa FALHOU!");
        while (1);
    }
    printf("Modulo LoRa OK.\n");

    lora_on_receive(on_lora_receive);
    servo_set_angle(0);
    current_state = STATE_CLOSED;
    last_move_time_ms = to_ms_since_boot(get_absolute_time());

    printf("Inicializacao completa. Aguardando comandos...\n");
    // A UI será atualizada pelo loop principal

    while (true) {
        if (received_command != CMD_NONE) {
            Command cmd_to_process;
            gpio_set_irq_enabled(LORA_INTERRUPT_PIN, GPIO_IRQ_EDGE_RISE, false);
            cmd_to_process = received_command;
            received_command = CMD_NONE;
            gpio_set_irq_enabled(LORA_INTERRUPT_PIN, GPIO_IRQ_EDGE_RISE, true);
            
            // --- Lógica da Máquina de Estados ---
        switch(cmd_to_process) {
            case CMD_OPEN:
                if (current_state == STATE_CLOSED || current_state == STATE_STOPPED) {
                    current_state = STATE_OPENING;
                }
                break;

            case CMD_STOP:
                // <<< LÓGICA ATUALIZADA para parar o fechamento também >>>
                if (current_state == STATE_OPENING || current_state == STATE_CLOSING) {
                    current_state = STATE_STOPPED;
                } 
                // Se já está parado, o comando de "OPEN" o faz continuar, 
                // então não precisamos da lógica de "retomar" aqui.
                break;
                
            case CMD_CLOSE:
                // <<< LÓGICA ATUALIZADA >>>
                // Se o portão não estiver já fechado, inicia o processo de fechamento gradual.
                if (current_state != STATE_CLOSED) {
                    current_state = STATE_CLOSING;
                }
                break;
                
            default: 
                break;
            }
        }
        
        // --- Lógica de Movimento Gradual do Servo ---
        if (current_state == STATE_OPENING) {
            uint32_t current_time_ms = to_ms_since_boot(get_absolute_time());
            if ((current_time_ms - last_move_time_ms > 20) && (servo_angle < 180)) {
                servo_angle++;
                servo_set_angle(servo_angle);
                last_move_time_ms = current_time_ms;
                if (servo_angle >= 180) {
                    current_state = STATE_OPEN;
                }
            }
        }

                // --- Lógica de Movimento Gradual do Servo ---
        if (current_state == STATE_CLOSING) {
            uint32_t current_time_ms = to_ms_since_boot(get_absolute_time());
            if ((current_time_ms - last_move_time_ms > 20) && (servo_angle > 0)) {
                servo_angle--;
                servo_set_angle(servo_angle);
                last_move_time_ms = current_time_ms;
                if (servo_angle <= 0) {
                    current_state = STATE_CLOSED;
                }
            }
        }
        
        // --- Atualização da UI (Display e LED) ---
        char line1[20], line2[20];
        switch(current_state) {
            case STATE_CLOSING: sprintf(line1, ">> Fechando >>");  rgb_led_set_color(COR_LED_VERMELHO); break;
            case STATE_CLOSED:  sprintf(line1, "Portao Fechado"); rgb_led_set_color(COR_LED_AZUL); break;
            case STATE_OPENING: sprintf(line1, ">> Abrindo >>");  rgb_led_set_color(COR_LED_VERDE); break;
            case STATE_STOPPED: sprintf(line1, "|| Parado ||");   rgb_led_set_color(COR_LED_AMARELO); break;
            case STATE_OPEN:    sprintf(line1, "Portao Aberto");  rgb_led_set_color(COR_LED_CIANO); break;
        }
        sprintf(line2, "Angulo: %d", servo_angle);
        display_update_gate_status(&display, line1, line2);

        sleep_ms(10);
    }
}