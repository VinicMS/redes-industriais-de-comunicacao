/* Definições das bibliotecas */
#include <Modbusino.h>
#include <Ultrasonic.h>

//Quantidade de memórias (registradores)

uint16_t tab_reg[6];
/* TABELA DE CORRESPONDÊNCIA NO SCADA
 *  
 * BOTAO_FISICO:  1 - tab_reg[0]
 * LED_FISICO:    2 - tab_reg[1]
 * BOTAO_VIRTUAL: 3 - tab_reg[2]
 * LED_VIRTUAL:   4 - tab_reg[3]
 * ULTRASSONICO:  5 - tab_reg[4]
 * MOTOR:         6 - tab_reg[5]
 * 
 */

//Portas e definições iniciais dos componentes (sensores/atuadores)
//virtual: que é acionado/comandado por um elemento do supervisorio SCADA.
//fisico: que é acionado/comandado por um elemento do hardware projetado fisicamente.

ModbusinoSlave modbusino_slave(42);
Ultrasonic ultrassom(4, 11);
const int porta_led_virtual = 7;
const int porta_led_fisico = 5;
const int porta_botao_fisico = 6;
const int porta_trigger_ultrassonico = 4;
const int porta_echo_ultrassonico = 11;
const int porta1_motor1 = 9;
const int porta2_motor1 = 10;

//Variáveis de projeto
int leitura_botao_virtual = 0;
int leitura_botao_fisico = 0;
int leitura_led_virtual = 0;
int leitura_led_fisico = 0;
int leitura_ultrassonico = 0;
int leitura_slider = 0; //representa a velocidade que será aplicada no motor

void setup() {
  
  modbusino_slave.setup(9600);

  //Comportamento dos pinos
  pinMode(porta_led_virtual, OUTPUT);
  pinMode(porta_led_fisico, OUTPUT);
  pinMode(porta_botao_fisico, INPUT);

  //Declaração do motor 1
  pinMode(porta1_motor1, OUTPUT);
  pinMode(porta2_motor1, OUTPUT);
}

void loop() {
  
  /* (1) Manipulação botão físico (escravo) */
  leitura_botao_fisico = digitalRead(porta_botao_fisico);

  if (leitura_botao_fisico == HIGH){
    leitura_led_fisico = digitalRead(porta_led_fisico);
    
    if(leitura_led_fisico == HIGH){
      digitalWrite(porta_led_fisico, LOW);
      tab_reg[0] = LOW;
    }
    if(leitura_led_fisico == LOW){
      digitalWrite(porta_led_fisico, HIGH);
      tab_reg[0] = HIGH;
    }
    delay(500);
  }

  /* (2) Exibicao do estado atual do led físico (escravo) */
  leitura_led_fisico = digitalRead(porta_led_fisico);
  tab_reg[1] = leitura_led_fisico;

  /* (3) Manipulação botão virtual (mestre) */
  leitura_botao_virtual = tab_reg[2];
  
  if (leitura_botao_virtual == 0){
    digitalWrite(porta_led_virtual, LOW);
  }
  else if (leitura_botao_virtual == 1){
    digitalWrite(porta_led_virtual, HIGH);
  }

  /* (4) Exibição do estado atual do led virtuall (mestre) */
  leitura_led_virtual = digitalRead(porta_led_virtual);
  tab_reg[3] = leitura_led_fisico;
  
  /* (5) Exibição do estado atual da leitura de distância (ultrassonico) */
  leitura_ultrassonico = ultrassom.Ranging(CM);
  tab_reg[4] = leitura_ultrassonico;

  /* (6) Manipulação do slider de velocidade do motor (mestre) */
  leitura_slider = tab_reg[5];
  Serial.println(leitura_slider);
  
  if(leitura_slider == 0){
    digitalWrite(porta1_motor1, LOW);
    digitalWrite(porta2_motor1, LOW);
  }
  else if(leitura_slider > 0){
    analogWrite(porta1_motor1, leitura_slider);
    digitalWrite(porta2_motor1, LOW);
  }
  else if(leitura_slider < 0){
    analogWrite(porta1_motor1, LOW);
    digitalWrite(porta2_motor1, leitura_slider*(-1));
  }
  
  //Atualiza o status no servidor (mestre)
  modbusino_slave.loop(tab_reg, 6);
}
