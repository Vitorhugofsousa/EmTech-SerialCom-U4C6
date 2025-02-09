    //adicionando bibliotecas
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "Serial_Com.pio.h"
#include "inc/font.h"
#include "inc/ssd1306.h"

//DEFINIÇÕES dos pinos e porta I2C
#define LED_PIN_GREEN 11 //led verde
#define LED_PIN_BLUE 12 //led azul
#define LED_PIN_RED 13 //led vermelho
#define NUM_PIXELS 25 //número de leds na matriz
#define LED_PIN 7 //pino de saída da matriz de led
#define BOTAO_A 5 //pino saida botao a
#define BOTAO_B 6 //pino saida botao b
#define I2C_PORT i2c1 //porta I2C
#define I2C_SDA 14 //pino SDA
#define I2C_SCL 15 //pino SCL
#define display_address 0x3C //endereço do display

//VARIÁVEIS GLOBAIS
int static volatile indice = 0; //variável para countrole do índice da matriz de led
uint count = 0; //variável para countrole do countador
uint actual_time = 0; //variável para countrole do tempo
uint valor_led; //variável para countrole do valor do led
uint sm;  //variável para countrole do state machine
PIO pio = pio0;  //variável para countrole da pio
ssd1306_t ssd;  //variável para countrole do display
bool GREEN_LED_OFF = false; //variável para countrole do led verde
bool BLUE_LED_OFF = false;  //variável para countrole do led azul
bool RED_LED_OFF = false;   //variável para countrole do led vermelho
double r = 0.3, g = 0.3, b = 0.3;  // Variáveis para controlar a intensidade de cada cor
int numero_anterior = -1; // Variável para armazenar o número anterior

// MATRIZ DE LEDS
// ROTINA PARA DEFINIÇÃO DA INTENSIDADE DE CORES DO LED
uint matrix_rgb(float r, float g, float b){
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
  }
  
  // FUNÇÃO PARA CONVERTER A POSIÇÃO DO MATRIZ PARA UMA POSIÇÃO DO VETOR
  int getIndex(int x, int y){
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda
    if (y % 2 == 0){
      return 24 - (y * 5 + x); // Linha par (esquerda para direita)
    }else{
      return 24 - (y * 5 + (4 - x)); // Linha ímpar (direita para esquerda)
    }
  }
  
  
  // FUNCAO PARA DESENHAR A MATRIZ
  void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(desenho[i] * r, desenho[i] * g, desenho[i] * b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}
  

//NUMEROS PARA EXIBIR NA MATRIZ DE LED

double numero0[25] = {    //Número 0
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 1.0, 1.0,
    1.0, 0.0, 1.0, 0.0, 1.0,
    1.0, 1.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double numero1[25] = {    //Número 1
    0.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0};
  
double numero2[25] = {    //Número 2
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double numero3[25] = {    //Número 3
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double numero4[25] = {    //Número 4
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0};
  
double numero5[25] = {    //Número 5
    1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double numero6[25] = {    //Número 6
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double numero7[25] = {    //Número 7
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double numero8[25] = {    //Número 8
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double numero9[25] = {    //Número 9
    1.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0};

double apagar_leds[25] ={   //Apagar LEDs da matriz
    0.0, 0.0, 0.0, 0.0, 0.0,          
    0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};

double *numeros[11] = {numero0, numero1, numero2, numero3, numero4, numero5, numero6, numero7, numero8, numero9, apagar_leds};  // Vetor com os números

// FUNÇÃO DE CALLBACK PARA OS BOTÕES
void callback_button(uint gpio, uint32_t events) {
    uint time = to_ms_since_boot(get_absolute_time());
    if (time - actual_time > 250) { // Condição para evitar múltiplos pressionamentos (debounce)
        actual_time = time;       // Atualiza o tempo
        if (gpio == BOTAO_A) {  // Verifica se o botão A foi pressionado
            GREEN_LED_OFF = !GREEN_LED_OFF; // Inverte o estado do LED verde
            gpio_put(LED_PIN_GREEN, GREEN_LED_OFF);  // Acende o LED verde
                    if (GREEN_LED_OFF == false){
                        
                        printf("LED verde desligado\n");
                    } else {
                        printf("LED verde ligado\n");
                    }
            
        } else if (gpio == BOTAO_B) { // Verifica se o botão B foi pressionado
            BLUE_LED_OFF = !BLUE_LED_OFF; // Inverte o estado do LED azul
            gpio_put(LED_PIN_BLUE, BLUE_LED_OFF);  // Acende o LED verde
                    if (BLUE_LED_OFF == false){
                            printf("LED azul desligado\n");
                        } else {
                            printf("LED azul ligado\n");
                        }
        }

    }

    // Atualiza o display
    ssd1306_fill(&ssd, false); // Limpa o display
    // Imprime estado atual dos leds no display
    gpio_get(LED_PIN_GREEN) ? ssd1306_draw_string(&ssd, "LED Verde ON", 10, 10) :
                             ssd1306_draw_string(&ssd, "LED Verde OFF", 10, 10);
    gpio_get(LED_PIN_BLUE) ? ssd1306_draw_string(&ssd, "LED Azul ON", 10, 30) :
                            ssd1306_draw_string(&ssd, "LED Azul OFF", 10, 30);
    ssd1306_send_data(&ssd); // Atualiza o display
}

int main(){
    bool frequenciaClock; // Variável para verificar se a frequência do clock foi configurada corretamente
    uint16_t i; // Variável para controlar o loop
    pio = pio0; // Seleciona a PIO 0
    uint32_t valor_led = 0; // Inicializa com preto (todos os LEDs apagados)
    char c; // Variável para armazenar o caractere recebido no monitor serial
    frequenciaClock = set_sys_clock_khz(128000, false); // Configura a frequência do clock para 128 MHz

    // Inicialização de todos os periféricos
    stdio_init_all(); 
    gpio_init(LED_PIN);
    gpio_init(LED_PIN_RED);
    gpio_init(LED_PIN_GREEN);
    gpio_init(LED_PIN_BLUE);
    gpio_init(BOTAO_A);
    gpio_init(BOTAO_B);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_pull_up(BOTAO_B);
    gpio_put(GREEN_LED_OFF, false);
    gpio_put(BLUE_LED_OFF, false);
    gpio_put(RED_LED_OFF, false);
    
    // Inicialização e configuração do display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA); 
    gpio_pull_up(I2C_SCL); 
    ssd.i2c_port = I2C_PORT;
    ssd.address = display_address;
    ssd.width = 128;
    ssd.height = 64;
    ssd.external_vcc = false;
    ssd1306_init(&ssd, 128, 64, false, display_address, I2C_PORT);
    ssd1306_config(&ssd);

    
    //vericação da frequência do clock
    printf("iniciando a transmissão PIO");
    if (frequenciaClock){
      printf("clock set to %ld\n", clock_get_hz(clk_sys));
    }else if(!frequenciaClock){
      printf("erro ao configurar a frequencia do clock");
    }
    
    
    // Inicializa a matriz de LED
    uint offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, LED_PIN);
    desenho_pio(apagar_leds, valor_led, pio, sm, r, g, b); // Apaga os LEDs ao iniciar o programa

    // Configuração dos botões
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &callback_button);  
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &callback_button);  

    // Loop principal
    while (true) {
        int c = getchar_timeout_us(0); // Verifica se há caractere disponível sem bloquear

        if (c != PICO_ERROR_TIMEOUT) { // Se um caractere foi recebido
            printf("Caractere recebido: %c\n", (char)c);
            int numero = c - '0';

            ssd1306_fill(&ssd, false);  // Limpa o display
            ssd1306_draw_char(&ssd, (char)c, 20, 30); // Usando ssd1306_draw_char
            ssd1306_send_data(&ssd);    // Atualiza o display

            if (numero != numero_anterior) {    // Verifica se o número é diferente do anterior
                numero_anterior = numero;    // Atualiza o número anterior

                if (numero >= 0 && numero <= 9) {   // Verifica se o número é válido
                    desenho_pio(numeros[numero], valor_led, pio, sm, r, g, b);  // Desenha o número na matriz de LED
                } else {
                    desenho_pio(apagar_leds, valor_led, pio, sm, r, g, b);  // Apaga os LEDs
                    printf("Caractere inválido!\n");    // Imprime mensagem de erro
                }
            }
        }
    }
}
