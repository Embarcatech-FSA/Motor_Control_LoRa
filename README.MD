
---

# Controle Remoto de Servo Motor via LoRa com Raspberry Pi Pico

![Licença](https://img.shields.io/badge/license-MIT-blue.svg)

Este repositório contém o código-fonte completo para um sistema de controle remoto de longo alcance, utilizando o protocolo LoRa para comunicação entre duas placas Raspberry Pi Pico. O projeto é dividido em dois módulos principais: um **Transmissor (Controle Remoto)** com botões e um **Receptor (Atuador)** que controla um servo motor.

Esta implementação foi desenvolvida como parte da residência em Software Embarcado do EmbarcaTech.

## 💡 Aplicação

O sistema simula o controle de um portão automatizado. Ele permite:
-   **Abrir gradualmente** o portão (movendo o servo de 0° a 180°).
-   **Pausar e continuar** o movimento de abertura a qualquer momento.
-   **Fechar o portão** imediatamente, retornando o servo à sua posição inicial.

A utilização do LoRa garante uma comunicação robusta e de longo alcance, ideal para aplicações em automação residencial, rural (controle de porteiras) ou industrial, onde o Wi-Fi ou Bluetooth podem não ter alcance ou penetração de sinal suficientes.

## ✨ Funcionalidades

-   **Comunicação de Longo Alcance:** Utiliza módulos LoRa RFM95W para comunicação confiável a centenas de metros de distância.
-   **Módulo Transmissor (Controle Remoto):**
    -   Três botões para as funções de Abrir, Parar/Continuar e Fechar.
    -   Detecção de pressionamento de botão por interrupções para alta eficiência energética.
    -   Lógica de *debounce* de software para evitar leituras falsas.
    -   Display OLED para feedback visual do último comando enviado.
-   **Módulo Receptor (Atuador):**
    -   Controla um servo motor de 180° usando PWM.
    -   Implementa uma máquina de estados (`FECHADO`, `ABRINDO`, `PARADO`, `ABERTO`) para gerenciar o movimento.
    -   Display OLED para exibir o status atual e o ângulo do portão em tempo real.
    -   LED RGB para indicar o status do sistema (Pronto, Em Operação, Parado).
-   **Código Modular:** O código é escrito em C com o Pico SDK e organizado em drivers modulares para fácil manutenção e entendimento.

## 🎥 Vídeo de Demonstração

Assista ao sistema em funcionamento no nosso vídeo de apresentação:

**[Controle de Motor com LoRa - Apresentação](https://youtu.be/dPm-mPDBAAs)**

## 🛠️ Hardware Necessário

| Módulo Transmissor (TX)             | Módulo Receptor (RX)                 |
| ------------------------------------- | -------------------------------------- |
| 1x Raspberry Pi Pico (ou Pico W)      | 1x Raspberry Pi Pico (ou Pico W)       |
| 1x Módulo LoRa RFM95W (SPI)           | 1x Módulo LoRa RFM95W (SPI)            |
| 3x Botões (Push-buttons)              | 1x Servo Motor 180° (ex: SG90, MG996R) |
| 1x Display OLED 128x64 (I2C)          | 1x Display OLED 128x64 (I2C)           |
| 1x Protoboard e Jumpers               | 1x LED RGB (Catodo Comum)              |
|                                       | 1x Protoboard e Jumpers                |
|                                       | 3x Resistores (~220Ω) para o LED       |
|                                       | **1x Fonte de 5V externa para o servo**¹ |

¹**Importante:** Não alimente o servo motor diretamente do pino `VBUS` da Pico, pois ele pode consumir mais corrente do que a porta USB do seu computador consegue fornecer, causando reinicializações da placa. Use uma fonte externa de 5V e conecte os terras (GND) da fonte e da Pico.

## 📁 Estrutura do Repositório

O código está organizado com os dois projetos (`transmitter` e `receptor`) na raiz do repositório, cada um com seus próprios arquivos de driver e configuração para facilitar a compilação independente.


```
.
├── transmitter/              # Código do Controle Remoto
│   ├── main.c
│   ├── config.h
│   ├── buttons.h & .c
│   ├── debouncer.h & .c
│   ├── display.h & .c
│   └── ...
│
├── receptor/                 # Código do Controlador do Servo
│   ├── main.c
│   ├── config.h
│   ├── servo.h & .c
│   ├── led_rgb.h & .c
│   ├── display.h & .c
│   └── ...
│
├── common/                   # Drivers compartilhados
│   └── lora.h & .c
│
└── ... (outros arquivos, como CMakeLists.txt)
```

## 🚀 Setup e Compilação

Este projeto utiliza o **Pico SDK**. Certifique-se de que seu ambiente de desenvolvimento esteja configurado corretamente.

1. **Pré-requisitos:** Tenha o [Pico SDK](https://github.com/raspberrypi/pico-sdk), `arm-none-eabi-gcc` e `cmake` instalados e configurados em suas variáveis de ambiente.
2. **Clonar o Repositório:**

   ```bash
   git clone https://github.com/Embarcatech-FSA/Motor_Control_LoRa.git
   cd Motor_Control_LoRa
   ```
3. **Compilar o Projeto:**

   ```bash
   mkdir build
   cd build
   cmake ..
   ninja  # Ou 'make' se você não estiver usando ninja
   Este comando irá compilar **ambos os projetos**. Ao final, você terá dois arquivos na pasta `build`:
   - `transmitter/transmitter.uf2`
   - `receptor/receptor.uf2`
   ```
4. **Carregar o Firmware:**

   - Conecte a Pico do **transmissor** ao computador segurando o botão `BOOTSEL`.
   - Arraste o arquivo `transmitter/transmitter.uf2` para a unidade `RPI-RP2` que aparecer.
   - Faça o mesmo para a Pico do **receptor**, mas usando o arquivo `receptor/receptor.uf2`.

## ⚙️ Configuração dos Pinos

Toda a pinagem dos periféricos pode ser alterada nos arquivos `config.h` localizados dentro das pastas `transmitter` e `receptor`, respectivamente. Certifique-se de que a sua fiação corresponda à configuração definida nesses arquivos.

## 📜 Licença

Este projeto está sob a licença MIT.
