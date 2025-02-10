#include <stdio.h>
#include <ctype.h> // Para verificação de caracteres
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "pio_matrix.pio.h"
#include "ssd1306.h"
#include "font.h"

// Definições de pinos
#define pin_blue 12 
#define pin_green 11

#define botaoA 5
#define botaoB 6

#define I2C_PORT i2c1
#define ADDR 0x3C
#define I2C_SDA 14
#define I2C_SCL 15

#define NUM_PIXELS 25
#define matriz5x5 7

// Variáveis globais para controle dos botões e display
volatile bool botaoA_press = false; 
volatile bool botaoB_press = false;
ssd1306_t ssd;

// Declaração de funções
void button_isr(uint gpio, uint32_t events); 
void setup();
uint32_t matrix_rgb(double r, double g, double b);
void apagar_leds(PIO pio, uint sm);
void acender_leds(PIO pio, uint sm, double r, double g, double b);
void desenho_pio(double *desenho, PIO pio, uint sm, double r, double g, double b);
void imprimir_num(char caracter, PIO pio, uint sm);

// Matrizes dos números (0 a 9)
double num0[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};
double num1[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.2, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0};
double num2[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};
double num3[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};
double num4[25] = {0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0};
double num5[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};
double num6[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};
double num7[25] = {0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};
double num8[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};
double num9[25] = {0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0, 0.0, 0.2, 0.0, 0.2, 0.0, 0.0, 0.2, 0.2, 0.2, 0.0};

// Função principal
int main() {
    PIO pio = pio0; // Inicializa o bloco PIO 0
    uint sm = 0;    // Máquina de estado 0
    uint offset = pio_add_program(pio, &pio_matrix_program); // Carrega o programa PIO
    pio_matrix_program_init(pio, sm, offset, matriz5x5);     // Inicializa a matriz 5x5

    setup();        // Configura o hardware
    stdio_init_all(); // Inicializa a comunicação USB/serial

    while (true) {
        if (stdio_usb_connected()) { // Verifica se há conexão com o Serial Monitor
            char character = getchar(); // Lê um caractere do terminal

            // Verifica se o caractere é um dígito
            if (isdigit(character)) {
                imprimir_num(character, pio, sm); // Exibe o número na matriz
                printf("Número: %c\n", character); // Exibe no Serial Monitor

                // Atualiza o display
                ssd1306_fill(&ssd, false);
                ssd1306_draw_char(&ssd, character, 64, 32); // Centraliza o caractere
                ssd1306_send_data(&ssd);
            } else {
                apagar_leds(pio, sm); // Limpa a matriz de LEDs
                printf("Caracter: %c\n", character); // Exibe no Serial Monitor

                // Atualiza o display
                ssd1306_fill(&ssd, false);
                ssd1306_draw_char(&ssd, character, 64, 32);
                ssd1306_send_data(&ssd);
            }
        }
        sleep_ms(50); // Pausa para reduzir o uso da CPU
    }
}

// Função de interrupção para os botões
void button_isr(uint gpio, uint32_t events) {
    static absolute_time_t last_press = 0; // Último tempo de pressionamento
    absolute_time_t now = get_absolute_time();

    // Debounce de 200ms (evita leituras múltiplas)
    if (absolute_time_diff_us(last_press, now) > 240000) {
        if (gpio == botaoA) { // Botão A pressionado
            botaoA_press = !botaoA_press; // Alterna o estado
            gpio_put(pin_green, botaoA_press); // Atualiza o LED verde

            // Atualiza o display
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, botaoA_press ? "LED verde on" : "LED verde off", 10, 30);
            ssd1306_send_data(&ssd);

            printf("LED verde: %s\n", botaoA_press ? "ligado" : "desligado");
        } else if (gpio == botaoB) { // Botão B pressionado
            botaoB_press = !botaoB_press; // Alterna o estado
            gpio_put(pin_blue, botaoB_press); // Atualiza o LED azul

            // Atualiza o display
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, botaoB_press ? "LED azul on" : "LED azul off", 10, 30);
            ssd1306_send_data(&ssd);

            printf("LED azul: %s\n", botaoB_press ? "ligado" : "desligado");
        }
        last_press = now; // Atualiza o tempo do último evento
    }
}

// Função de configuração do hardware
void setup() {
    // Configura os LEDs
    gpio_init(pin_blue);
    gpio_set_dir(pin_blue, GPIO_OUT);
    gpio_init(pin_green);
    gpio_set_dir(pin_green, GPIO_OUT);

    // Configura os botões
    gpio_init(botaoA);
    gpio_set_dir(botaoA, GPIO_IN);
    gpio_pull_up(botaoA);
    gpio_set_irq_enabled_with_callback(botaoA, GPIO_IRQ_EDGE_FALL, true, &button_isr);

    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &button_isr);

    // Configura o I2C para o display
    i2c_init(I2C_PORT, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);

    // Inicializa e configura o Display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Função para converter valores RGB em um valor de 32 bits no formato específico da matriz.
uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Função para apagar todos os LEDs
void apagar_leds(PIO pio, uint sm) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio, sm, matrix_rgb(0.0, 0.0, 0.0)); // Preto = desligado
    }
}

// Função para acender todos os LEDs com uma cor específica
void acender_leds(PIO pio, uint sm, double r, double g, double b) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio, sm, matrix_rgb(r, g, b));
    }
}

// Função para desenhar um padrão na matriz de LEDs
void desenho_pio(double *desenho, PIO pio, uint sm, double r, double g, double b) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio, sm, matrix_rgb(desenho[i] * r, desenho[i] * g, desenho[i] * b));
    }
}

// Função para exibir um número na matriz de LEDs
void imprimir_num(char caracter, PIO pio, uint sm) {
    switch (caracter) {
        case '0': desenho_pio(num0, pio, sm, 0.5, 0.6, 0.2); break;
        case '1': desenho_pio(num1, pio, sm, 0.5, 0.6, 0.2); break;
        case '2': desenho_pio(num2, pio, sm, 0.5, 0.6, 0.2); break;
        case '3': desenho_pio(num3, pio, sm, 0.5, 0.6, 0.2); break;
        case '4': desenho_pio(num4, pio, sm, 0.5, 0.6, 0.2); break;
        case '5': desenho_pio(num5, pio, sm, 0.5, 0.6, 0.2); break;
        case '6': desenho_pio(num6, pio, sm, 0.5, 0.6, 0.2); break;
        case '7': desenho_pio(num7, pio, sm, 0.5, 0.6, 0.2); break;
        case '8': desenho_pio(num8, pio, sm, 0.5, 0.6, 0.2); break;
        case '9': desenho_pio(num9, pio, sm, 0.5, 0.6, 0.2); break;
        default: break;
    }
}