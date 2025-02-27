#include <stdio.h>
#include "pico/stdlib.h"    
#include "pico/time.h"      
#include "hardware/irq.h"   
#include "hardware/pwm.h"   
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/adc.h"     
#include <string.h>

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_ADDR 0x3C

#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12
#define BOTAO_A 5
#define BOTAO_B 6
#define JOYSTICK_BOTAO 22
#define JOYSTICK_X 26
#define JOYSTICK_Y 27
#define DEAD_ZONE 1000
#define ADC_CENTER 2048
const float PWM_DIV = 100.0;     
const uint16_t PWM_WRAP = 25000;

volatile bool status_led_verde = false;   
volatile bool pwm_ligado = true;         
static volatile uint a = 1;
static volatile uint32_t ultimo_tempo = 0; 
static volatile uint32_t ultimo_tempo_joystick = 0; 

uint16_t RECT_Y_MIN = 4;
uint16_t RECT_Y_MAX = 52;
uint16_t RECT_X_MIN = 4;
uint16_t RECT_X_MAX = 116;
uint8_t borda = 0;  
uint16_t borda_1 = 1;
uint16_t borda_2 = 60;
uint16_t borda_3 = 1;
uint16_t borda_4 = 124;
uint16_t t = 0;
bool aumenta = true;
uint16_t pwm_vermelho = 0;
uint16_t pwm_verde = 10000;
uint16_t pwm_azul = 0;
bool pedido_em_preparacao = false;
char cor[10] = "Verde";
char str[2] = "00";
bool ponteiro = false;
bool click_joystick = false;


uint16_t minutos = 10;
uint16_t segundos = 00;

ssd1306_t display;

void init_io();

void cor_branca() {
    pwm_vermelho = 10000;
    pwm_verde = 10000;
    pwm_azul = 10000;
    strcpy(cor, "Branca");
}
void cor_amarela() {
    pwm_vermelho = 10000;
    pwm_verde = 10000;
    pwm_azul = 0;
    strcpy(cor, "Amarela");
}

void cor_verde() {
    pwm_vermelho = 0;
    pwm_verde = 10000;
    pwm_azul = 0;
    strcpy(cor, "Verde");
}

void cor_vermelha() {
    pwm_vermelho = 10000;
    pwm_verde = 0;
    pwm_azul = 0;
    strcpy(cor, "Vermelha");
}
void mensagem(uint16_t status) {
    ssd1306_fill(&display, false);
    ssd1306_draw_string(&display, "MESA 01", 40, 4);

    switch (status) {
        case 1:
            ssd1306_draw_string(&display, "LIVRE", 45, 30);
            break;
        case 2:
            ssd1306_draw_string(&display, "OCUPADA", 40, 30);
            break;
        case 3:
            ssd1306_draw_string(&display, "EM ATENDIMENTO", 10, 30);
            break;
        case 4:
            ssd1306_draw_string(&display, "AGUARDANDO", 28, 30);
            sprintf(str, "TEMPO %02d:%02d", minutos, segundos);
            ssd1306_draw_string(&display, str, 20, 55);
            break;
        case 5:
            ssd1306_draw_string(&display, "CONCLUIDO", 32, 30);
            break;
        case 6:
            ssd1306_draw_string(&display, "ATRASADO", 36, 30);
        break;

        default:
            break;
    }
    ssd1306_send_data(&display);
}

int64_t turn_on_red_callback(alarm_id_t id, void *user_data) {
    if(pedido_em_preparacao) {
        // Desliga o LED configurando o pino LED_PIN para nível baixo.
        cor_vermelha();

        pwm_set_gpio_level(LED_VERMELHO, pwm_vermelho);
        pwm_set_gpio_level(LED_VERDE, pwm_verde);
        pwm_set_gpio_level(LED_AZUL, pwm_azul);
        mensagem(6);
        // Retorna 0 para indicar que o alarme não deve se repetir.
    }
    return 0;
}


//Função para aplicar tratamento das interrupções dos botões
static void gpio_interruptor(uint gpio, uint32_t events)
{
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());

    //Verificação do debounce de 300 ms
    if (tempo_atual - ultimo_tempo > 300000) {
        ultimo_tempo = tempo_atual;

        // Verificação do botão do joystick e alteração do estado do LED verde
        if (gpio == JOYSTICK_BOTAO && strcmp(cor, "Amarela") == 0) {
            click_joystick = !click_joystick;

            if(!click_joystick) { 
                uint16_t tempo_alarme = (minutos * 60) + segundos;
                add_alarm_in_ms(tempo_alarme * 1000, turn_on_red_callback, NULL, false);
                pedido_em_preparacao = true;
                mensagem(4);
                cor_branca();

                pwm_set_gpio_level(LED_VERMELHO, pwm_vermelho);
                pwm_set_gpio_level(LED_VERDE, pwm_verde);
                pwm_set_gpio_level(LED_AZUL, pwm_azul);
            }
        }

        if (gpio == BOTAO_A) {
            if (pwm_verde > 0 && pwm_vermelho == 0 && pwm_azul == 0) {
                cor_branca();
                mensagem(2);
            }
            else {
                cor_verde();
                pedido_em_preparacao = false;
                mensagem(1);
            }
            pwm_set_gpio_level(LED_VERMELHO, pwm_vermelho);
            pwm_set_gpio_level(LED_VERDE, pwm_verde);
            pwm_set_gpio_level(LED_AZUL, pwm_azul);
        } else if (gpio == BOTAO_B && strcmp(cor, "Verde") != 0) {
            if (strcmp(cor, "Vermelha") == 0) {
                cor_branca();
                pedido_em_preparacao = false;
                mensagem(5);
            }  else if (pedido_em_preparacao) {
                pedido_em_preparacao = false;
                mensagem(5);
            }
            else {
                cor_amarela();
                mensagem(3);
            }
            pwm_set_gpio_level(LED_VERMELHO, pwm_vermelho);
            pwm_set_gpio_level(LED_VERDE, pwm_verde);
            pwm_set_gpio_level(LED_AZUL, pwm_azul);
        }
    }
}
//Função para configurar o PWM de um pino
void configuracao_pwm(uint pino, uint funcao_gpio, float clkdiv, uint16_t wrap, uint16_t nivel) {
    // Configura o pino como PWM e define a função de GPIO
    gpio_set_function(pino, funcao_gpio);
    // Obtém o número do slice PWM correspondente ao pino
    uint slice_num = pwm_gpio_to_slice_num(pino);
    // Configura o divisor de clock
    pwm_set_clkdiv(slice_num, clkdiv);
    // Configura o wrap
    pwm_set_wrap(slice_num, wrap);
    // Configura o nível do PWM
    pwm_set_gpio_level(pino, nivel);
    // Habilita o PWM
    pwm_set_enabled(slice_num, true);
}

//Função para inicializar leds
void init_gpio_led(int led_pin, bool is_output, bool status) {
    gpio_init(led_pin);                                         // Inicializa o pino do LED
    gpio_set_dir(led_pin, is_output ? GPIO_OUT : GPIO_IN);      // Configura o pino como saída ou entrada
    gpio_put(led_pin, status);                                   // Garante que o LED inicie apagado
}

//Função para inicializar botões
void init_gpio_button(int button_pin, bool is_output) {
    gpio_init(button_pin);                                          // Inicializa o botão
    gpio_set_dir(button_pin, is_output ? GPIO_OUT : GPIO_IN);       // Configura o pino como entrada ou saída
    gpio_pull_up(button_pin);                                       // Habilita o pull-up interno
}

int main() {
    // Inicializa comunicação USB CDC para monitor serial
    stdio_init_all(); 
  
    // Inicializando pino do LED RGB
    init_gpio_led(LED_VERMELHO, true, false);
    init_gpio_led(LED_AZUL, true, false);
    init_gpio_led(LED_VERDE, true, false);
  
    // Inicializando pino dos botões (Botão A, Botão B e Botão do Joystick)
    init_gpio_button(BOTAO_A, false);
    init_gpio_button(BOTAO_B, false);
    init_gpio_button(JOYSTICK_BOTAO, false);
  
    // Inicializando pino do Joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);
  
    // Configuração do PWM para os LEDs RGB
    configuracao_pwm(LED_AZUL, GPIO_FUNC_PWM, PWM_DIV, PWM_WRAP, pwm_azul);
    configuracao_pwm(LED_VERMELHO, GPIO_FUNC_PWM, PWM_DIV, PWM_WRAP, pwm_vermelho);
    configuracao_pwm(LED_VERDE, GPIO_FUNC_PWM, PWM_DIV, PWM_WRAP, pwm_verde);
  
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_interruptor);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_interruptor);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BOTAO, GPIO_IRQ_EDGE_FALL, true, &gpio_interruptor);
  
    // Inicializa a comunicação I2C (400 kHz)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
  
    // Inicializa e configura o display OLED
    ssd1306_init(&display, WIDTH, HEIGHT, false, I2C_ADDR, I2C_PORT);
    ssd1306_config(&display);
    ssd1306_send_data(&display);
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);
    mensagem(1);
  
    // Variáveis para controle do piscar
    bool campo_visivel = true;  // Controla se o campo ativo está visível
    uint32_t ultimo_piscar = 0;  // Última vez que o piscar mudou
    const uint32_t intervalo_piscar = 150000;  // Intervalo de 500ms para piscar
  
    while (1) {

        if(click_joystick) {

            ssd1306_fill(&display, false);
            // Leitura dos valores do ADC para os eixos Y e X
            adc_select_input(0); // Eixo Y (pino 27)
            uint16_t adc_y_value = adc_read();
            adc_select_input(1); // Eixo X (pino 26)
            uint16_t adc_x_value = adc_read();
    
            uint32_t tempo_atual = to_us_since_boot(get_absolute_time());
    
            // Controle do eixo X (seleção de minutos ou segundos)
            if (adc_x_value > (ADC_CENTER + DEAD_ZONE)) {
                if (tempo_atual - ultimo_tempo_joystick > 300000) {
                    ultimo_tempo_joystick = tempo_atual;
                    ponteiro = true;  // Seleciona segundos
                }
            } else if (adc_x_value < (ADC_CENTER - DEAD_ZONE)) {
                if (tempo_atual - ultimo_tempo_joystick > 300000) {
                    ultimo_tempo_joystick = tempo_atual;
                    ponteiro = false;  // Seleciona minutos
                }
            }
    
            // Controle do eixo Y (aumentar/diminuir)
            if (adc_y_value > (ADC_CENTER + DEAD_ZONE)) {
                if (tempo_atual - ultimo_tempo_joystick > 100000) {
                    ultimo_tempo_joystick = tempo_atual;
                    if (ponteiro == false) {
                        if (minutos == 59)
                        {
                            minutos = 0;
                        } else {
                            minutos++;
                        }
                    } else {
                        if (segundos == 59)
                        {
                            segundos = 0;
                        } else {
                            segundos++;
                        }
                        
                    }
                }
            } else if (adc_y_value < (ADC_CENTER - DEAD_ZONE)) {
                if (tempo_atual - ultimo_tempo_joystick > 100000) {
                    ultimo_tempo_joystick = tempo_atual;
                    if (ponteiro == false) {
                        if (minutos == 0)
                        {
                            minutos = 59;
                        } else {
                            minutos--;
                        }
                    } else {
                        if (segundos == 0)
                        {
                            segundos = 59;
                        } else {
                            segundos--;
                        }
                    }
                }
            }
    
            // Controle do piscar
            if (tempo_atual - ultimo_piscar >= intervalo_piscar) {
                ultimo_piscar = tempo_atual;
                campo_visivel = !campo_visivel;  // Alterna visibilidade do campo ativo
            }
    
            // Atualiza o display
            ssd1306_fill(&display, false);
    
            // Exibe minutos e segundos separadamente
            char minutos_str[3];
            char segundos_str[3];
            sprintf(minutos_str, "%02d", minutos);
            sprintf(segundos_str, "%02d", segundos);
    
            // Minutos (pisca se ponteiro = false)
            if (ponteiro == false && campo_visivel) {
                ssd1306_draw_string(&display, minutos_str, 34, 30);
            } else if (ponteiro == true) {
                ssd1306_draw_string(&display, minutos_str, 34, 30);  // Sempre visível quando segundos estão ativos
            }
    
            // Dois pontos (sempre visível)
            ssd1306_draw_string(&display, ":", 52, 30);
    
            // Segundos (pisca se ponteiro = true)
            if (ponteiro == true && campo_visivel) {
                ssd1306_draw_string(&display, segundos_str, 60, 30);
            } else if (ponteiro == false) {
                ssd1306_draw_string(&display, segundos_str, 60, 30);  // Sempre visível quando minutos estão ativos
            }
    
            ssd1306_send_data(&display);

        }
  
        sleep_ms(100);
    }
  
    return 0;
  }