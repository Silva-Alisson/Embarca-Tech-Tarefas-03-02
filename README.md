# EmbarcaTech_Interfaces_ControleSerial
Tarefa do EmbarcaTech com intuito de consolidar os conceitos sobre o uso de interfaces de comunicação serial no RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab.

# Vídeo Explicativo
A seguir um vídeo explicativo sobre o projeto.

https://drive.google.com/file/d/1Byc8KjiFvhhaab0IXuegvTt3CftalbUv/view?usp=sharing

# Intruções
Para executar o código em questão, deve-se possuir a extensão Wokwi Simulator instalada no Visual Studio Code ou acesso a placa BitDogLab configurada para receber informação via Seria Monitor.

Ao ser executado, pressionando o Botão A, o LED de cor verde será ligado ou desligado e pressionando o Botão B, o LED azul será ligado ou desligado, com uma mensagem informativa sobre a ação realizada sendo exibida no display presente na placa e no Serial Monitor presente no Visual Studio Code.

Agora, utilizando o Serial Monitor, ao começar o monitoramento, será póssivel enviar caracteres, de A-Z, a-z e 0-9. Cada caracter enviado será exibido no display conectado a placa e também no Serial Monitor, se enviados números entre 0 e 9, além de exibidos nos meios já mencionados, serão também desenhados na matriz de LED 5x5.

## Tecnologias Utilizadas

- Raspberry Pi Pico  
- Matriz de LEDs controlada via PIO  
- Display OLED via I2C  
- Interrupções para leitura de botões físicos  
- Terminal Serial para entrada do usuário  

## Como Funciona

### Inicialização  
- Configura os pinos dos LEDs e botões.  
- Inicializa a comunicação I2C com o display OLED.  
- Configura a matriz de LEDs usando um programa PIO.  

### Loop Principal  
- Aguarda entrada do usuário via terminal serial.  
- Se for um número (0-9):  
  - Exibe na matriz de LEDs e no display OLED.  
- Se for outro caractere:  
  - Apaga a matriz de LEDs e exibe apenas no display OLED.  

### Interação com Botões  
- Dois botões físicos controlam LEDs (verde e azul).  
- Quando um botão é pressionado:  
  - O LED correspondente liga/desliga.  
  - O display OLED exibe o estado atualizado.

## Como Usar  

1. Conecte os componentes conforme o esquema elétrico.  
2. Suba o código para o Raspberry Pi Pico.  
3. Abra um terminal serial para enviar comandos.  
4. Pressione os botões físicos para alternar os LEDs.  