

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Carrega o SDK do Pico
include(pico_sdk_import.cmake)

project(receptor-lora C CXX ASM)
pico_sdk_init()

# Adiciona o executável e TODOS os arquivos fonte .c necessários
add_executable(${PROJECT_NAME}  
    main.c
    include/lib/ssd1306/ssd1306.c
    include/lora.c
    include/servo.c  
    include/led_rgb.c
    include/display.c
)

# Inclui o diretório raiz para que main.c possa encontrar "lora.h"
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# Liga as bibliotecas necessárias ao seu projeto
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    hardware_timer       
    hardware_spi      
    hardware_i2c
    hardware_pwm
    m            
)

# Habilita a saída de printf via USB e UART para depuração
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# Gera os arquivos de saída (.uf2, .elf, etc)
pico_add_extra_outputs(${PROJECT_NAME})