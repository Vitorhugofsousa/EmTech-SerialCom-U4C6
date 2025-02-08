#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"
#include "hardware/i2c.h"
#include "Serial_Com.pio.h"


#define LED_PIN_GREEN 11 //led verde
#define LED_PIN_BLUE 12 //led azul
#define LED_PIN_RED 13 //led vermelho
#define NUM_PIXELS 25 //número de leds na matriz
#define LED_PIN 7 //pino de saída da matriz de led
#define BOTAO_A 5 //pino saida botao a
#define BOTAO_B 6 //pino saida botao b
#define I2C_PORT i2c0
#define I2C_SDA 14  
#define I2C_SCL 15

//VARIÁVEIS GLOBAIS
int static volatile indice = 0; //variável para countrole do índice da matriz de led
uint count = 0; //variável para countrole do countador
uint actual_time = 0; //variável para countrole do tempo
uint valor_led; //variável para countrole do valor do led
uint sm;  //variável para countrole do state machine
PIO pio = pio0;  //variável para countrole da pio
bool GREEN_LED_OFF = false;
bool BLUE_LED_OFF = false;
bool RED_LED_OFF = false;

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
  void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){
    for (int16_t i = 0; i < NUM_PIXELS; i++)  // Percorre todos os LEDs da matriz
    {
      valor_led = matrix_rgb(desenho[i] * r, desenho[i] * g, desenho[i] * b); // Define a intensidade de cada cor
      pio_sm_put_blocking(pio, sm, valor_led);  // Atualiza o valor do LED
    };
  }
  

//NUMEROS PARA EXIBIR NA MATRIZ DE LED
double apagar_leds[25] ={   //Apagar LEDs da matriz
    0.0, 0.0, 0.0, 0.0, 0.0,          
    0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0};


void callback_button(uint gpio, uint32_t events) {
    uint time = to_ms_since_boot(get_absolute_time());
    if (time - actual_time > 250) { // Condição para evitar múltiplos pressionamentos (debounce)
        actual_time = time;       // Atualiza o tempo
        if (gpio == BOTAO_A) {  // Verifica se o botão A foi pressionado
            GREEN_LED_OFF = !GREEN_LED_OFF; // Inverte o estado do LED verde
            gpio_put(LED_PIN_GREEN, GREEN_LED_OFF);  // Acende o LED verde
        } else if (gpio == BOTAO_B) { // Verifica se o botão B foi pressionado
            BLUE_LED_OFF = !BLUE_LED_OFF; // Inverte o estado do LED azul
            gpio_put(LED_PIN_BLUE, BLUE_LED_OFF);  // Acende o LED verde
           
        }
    }
}

int main(){
    bool frequenciaClock; // Variável para verificar se a frequência do clock foi configurada corretamente
    uint16_t i; // Variável para controlar o loop
    float r, g, b;   // Variáveis para controlar a intensidade de cada cor
    pio = pio0; // Seleciona a PIO 0
    uint32_t valor_led = 0; // Inicializa com preto (todos os LEDs apagados)

    frequenciaClock = set_sys_clock_khz(128000, false); // Configura a frequência do clock para 128 MHz
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

    printf("iniciando a transmissão PIO");
    if (frequenciaClock){
      printf("clock set to %ld\n", clock_get_hz(clk_sys));
    }else if(!frequenciaClock){
      printf("erro ao configurar a frequencia do clock");
    }
      uint offset = pio_add_program(pio, &pio_matrix_program);
      sm = pio_claim_unused_sm(pio, true);
      pio_matrix_program_init(pio, sm, offset, LED_PIN);
    
    // Inicializa a matriz de LED
      desenho_pio(apagar_leds, valor_led, pio, sm, r, g, b); // Apaga os LEDs ao iniciar o programa

      // Configuração dos botões
      gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &callback_button);  
      gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &callback_button);  

    while (true) {
       
    }
}
