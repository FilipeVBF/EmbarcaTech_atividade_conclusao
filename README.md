# Sistema de Gerenciamento de Mesa com Temporizador

Este projeto tem como objetivo desenvolver um sistema de **Gerenciamento de Ocupação de Mesas para Restaurantes** utilizando a placa **BitDogLab**. O sistema consiste em um display OLED, LEDs RGB, botões e um joystick, com o intuito de melhorar a comunicação entre os clientes e os garçons, facilitando o processo de gestão das mesas em um restaurante.

## Funcionalidades

- **Status das Mesas**: As mesas podem ter seus status alterados entre *Livre*, *Ocupada*, *Em Atendimento*, *Aguardando* e *Concluído*.
- **Controle de Tempo**: A mesa pode mostrar o tempo de espera quando está *Aguardando* o pedido.
- **LEDs de Status**: LEDs RGB (vermelho, verde e azul) indicam visualmente o status da mesa.
- **Joystick**: Usado para selecionar e ajustar o tempo de espera para pedidos.
- **Botões**: Botões físicos são utilizados para alterar o status das mesas e chamar o garçom.
- **Alarme**: Um alarme é configurado para avisar quando um pedido está atrasado.
- **Display OLED**: Mostra o status das mesas e o tempo de espera.

## Arquitetura

O sistema é baseado em um modelo de camadas, com os seguintes principais blocos:

1. **Interface com o Hardware**: Leitura dos botões, joystick e controle de LEDs.
2. **Lógica de Controle**: Processa as entradas, gerencia os estados das mesas e controla o tempo de pedidos.
3. **Comunicação I2C**: Responsável pela comunicação entre o microcontrolador e o display OLED.
4. **Exibição**: Exibe as informações de status e tempo de espera no display OLED.
5. **Funções Auxiliares**: Funções como debounce de botões e controle de PWM para LEDs.

## Componentes Utilizados

- **BitDogLab**: Placa de desenvolvimento com microcontrolador.
- **Display OLED SSD1306**: Utilizado para exibir o status das mesas e o tempo de espera.
- **LEDs RGB**: Indicadores de status para as mesas (vermelho, verde e azul).
- **Botões**: Para interação com o sistema e alteração de status.
- **Joystick**: Para ajustar o tempo de espera.
- **Pinos de PWM**: Usados para controlar a intensidade dos LEDs.

## Funcionalidades do Sistema

1. **Mudança de Status**: O botão A alterna entre os estados *Livre* e *Ocupada* da mesa. O botão B altera entre *Aguardando* e *Concluído*.
2. **Controle de Tempo**: O joystick permite ajustar o tempo de espera (minutos e segundos) para pedidos aguardando atendimento.
3. **Alarme de Atraso**: Quando o tempo de espera atinge o limite configurado, um alarme é ativado, e a mesa muda para o estado *Atrasado*.
4. **LEDs Indicativos**: O sistema utiliza LEDs para indicar o status da mesa, com cores diferentes para cada situação (verde para livre, vermelho para ocupado, etc.).

## Como Executar o Projeto

1. Clone o repositório:
   git clone https://github.com/FilipeVBF/EmbarcaTech_atividade_conclusao.git
2. Importe a pasta do projeto para o ambiente de desenvolvimento do **Pico SDK**.
3. Conecte o hardware conforme a configuração sugerida.
4. Compile e carregue o código no **Raspberry Pi Pico W**.

## Exemplo de Conexões

- **Botões**:
  - BOTÃO_A: Pino 5
  - BOTÃO_B: Pino 6
  - JOYSTICK_BOTÃO: Pino 22
- **Joystick**:
  - JOYSTICK_X: Pino 26
  - JOYSTICK_Y: Pino 27
- **LEDs RGB**:
  - LED_VERMELHO: Pino 13
  - LED_VERDE: Pino 11
  - LED_AZUL: Pino 12
- **Display OLED (I2C)**:
  - SDA: Pino 14
  - SCL: Pino 15

## Vídeo da Solução
Segue abaixo o link do vídeo da demonstração do projeto:
[Ver vídeo](https://drive.google.com/file/d/1PQ610m6bIcYRiQXdW3t7iAfxOyUw7oth/view?usp=sharing).
