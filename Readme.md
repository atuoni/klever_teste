

# Programa para o desafio técnico da teste da Klever 

### Desenvolvido por Amauri Tuoni  - qua 24 mar 2021 20:00:16 


-Foi utilizado o MCU STM32 modelo STM32F407ZGT6 para os testes. 
-Foi utilizado o STM32CubeMX para auxiliar na configuração e inicialização dos periféricos da CPU.

-Acredito que consiga portar o código para o STM32F401RE sem dificuldades.

-O sistema opera através da USART2 @ 115200,N,8,1 nos pinos PA2 e PA3

-Foi Entrada analógica no pino PA0

-Foi utilizado LED no pino PA5


Os comandos utilizados para operar o sistema são:

### Ligar LED  => led on
### Desligar LED => led off
### Inverter (toggle) LED  => led tgl 
### Loop back (enviar de volta o mesmo pacote recebido) => lb on
### Loop back (desligar - não enviar de volta pacote recebido) => lb off 
### Ler conversor AD (valor mais recente, em 12 bits)  =>  adc


