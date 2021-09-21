// include the library code:
#include <LiquidCrystal.h>

// Blibioteca sensor temperatura e humidade
#include "DHT.h"
 
#define DHTPIN A1 // pino que estamos conectando o sensor DTH
#define DHTTYPE DHT11 // DHT 11 > tipo do DTH

#define sensorHumidade A4

// entrada analógica dos botões
#define Botoes A0

// Objeto DTH sensor temperatura
DHT dht(DHTPIN, DHTTYPE);

// --- Protótipo das Funções Auxiliares ---
void changeMenu(); //Função para modificar o menu atual
void dispMenu();   //Função para mostrar o menu atual
void lerBotoes();  //Função que le o estado dos botoes


void leTemperatura();

// --- Protótipo das Funções das telas ---
void tela_inicial();   //Função da tela inicial, data e hora atuais
void telaTemperatura(); //Função horário programável 1
void telaHumidadeSolo(); //Função horário programável 2
void telaUltimaIrrigacao(); //Função horário programável 3

// --- Variáveis Globais ---
int menu = 1; //Variável para selecionar o menu

boolean estado_butUP, estado_butDown, estado_butDireito, estado_butEsquerdo, estado_butSelect; //Flags para armazenar o estado dos botões

boolean funcao_butUp, funcao_butDown, funcao_butDireito, funcao_butEsquerdo, funcao_butSelect; //Flags para armazenar a função dos botões

// --- Variáveis das escolhas dos horários
byte ultimoDiaIrrigado = 0;
byte ultimoMesIrrigado = 0;
byte ultimaHoraIrrigado = 0; // Variável com valor inicial das horas
byte ultimoMinutoIrrigado = 0; //Variável com valor inicial dos minutos

// initialize the library with the numbers of the interface pins
LiquidCrystal disp(8, 9, 4, 5, 6, 7);

float h;
float t;

// --- Variavel que guarda a humidade do solo
int humidadeAnalogico;
int humidade;

// Objeto hora e data
RTCDateTime dataehora;

void setup()
{
    pinMode(Botoes, INPUT);
    pinMode(sensorHumidade, INPUT);

    // Inicia o display LCD
    disp.begin(16, 2);
    // Inicia serial
    Serial.begin(9600);

    Serial.println("DHTxx test!");

    // Inicia o DHT
    dht.begin();

    estado_butUP = 0x00; //limpa flag do botão Up
    estado_butDown = 0x00; //limpa flag do botão Down
    estado_butDireito = 0x00; //limpa flag do botão P
    estado_butEsquerdo = 0x00; //limpa flag do botão M

    
}

void loop()
{
    lerBotoes();  //Verifica o estado dos botões
    changeMenu(); //Muda a tela conforme a selecionada
    dispMenu();
    leTemperatura();
    leHumidadeSolo();
    Serial.println(analogRead(Botoes));
}

// --- Desenvolvimento das Funções Auxiliares ---
void lerBotoes()
{
    int valorbotao = analogRead(Botoes);
   
    if (valorbotao > 1008 && valorbotao < 1020)
        estado_butDireito = 0x01;             //Botão P pressionado? Seta flag
    if (valorbotao < 50 && estado_butDireito) //Botão P solto e flag setada?                         // BOTÃO LADO DIREITO
    {                                         //Se for sim...
        estado_butDireito = 0;                //Limpa flag
        funcao_butDireito = 1;                //Ativa função Botao p
    }
    else if (valorbotao > 340 && valorbotao < 372)
        estado_butEsquerdo = 0x01;             //Botão M pressionado? Seta flag
    if (valorbotao < 50 && estado_butEsquerdo) //Botão M solto e flag setada?                         // BOTÃO LADO ESQUERDO
    {                                          //Se for sim...
        estado_butEsquerdo = 0;                //Limpa flag
        funcao_butEsquerdo = 1;                //Ativa função Botao M
    }
    else if (valorbotao > 500 && valorbotao < 540)
        estado_butSelect = 0x01;             //Botão M pressionado? Seta flag
    if (valorbotao < 50 && estado_butSelect) //Botão M solto e flag setada?                         // BOTÃO SELECT
    {                                        //Se for sim...
        estado_butSelect = 0;                //Limpa flag
        funcao_butSelect = 1;                //Ativa função Botao M
    }

} // end lerBotoes

void changeMenu() //Função para modificar o menu atual
{

    if (funcao_butDireito)
    {                 //Se a função do botão DIREITO foi ATIVA
        disp.clear(); //Limpa display
        menu++;       //Incrementa o menu

        if (menu > 0x04)
            menu = 0x01; //Se menu maior que 6, volta a ser 1
        limpaFuncoes();  //Limpa função CIMA
    }                    //end butUp

    if (funcao_butEsquerdo)
    {                 //Se a função do botão BAIXO foi ATIVA                                                                 //Sim...
        disp.clear(); //Limpa display
        menu--;       //Decrementa o menu

        if (menu < 0x01)
            menu = 0x04; //Se menu menor que 1, volta a ser 6
        limpaFuncoes();  //Limpa função BAIXO
    }                    //end butDown

} //end changeMenu

void dispMenu() //Mostra o menu atual
{
    switch (menu) //Controle da variável menu
    {
    case 0x01:          //Caso 1
        tela_inicial(); //Chama a função da marca e da hora

        break;            //break
    case 0x02:            //Caso 4
        telaTemperatura(); //Chama função para o definir a quantidade

        break;            //break
    case 0x03:            //Caso 4
        telaHumidadeSolo(); //Chama função para o controle dos minutos

        break;            //break
    case 0x04:            //Caso 4
        telaUltimaIrrigacao(); //Chama função para o controle dos minutos

        break; //break
    }          //end switch menu
} //end dispMenu

void tela_inicial()
{
    disp.setCursor(0, 0); //Posiciona cursor na coluna 2, linha 1
    disp.print("Joana: a planta.");
    disp.setCursor(4, 1); //Posiciona cursor na coluna 2, linha 2
    disp.print("by Cesar");

    if (funcao_butDireito) //Se a função do botão P (DIREITA) for acionada
    {
        disp.setCursor(0, 1);           //Seta o cursor na segunda linha
        disp.print("                "); //"Apaga" a linha inteira
        disp.setCursor(2, 1);           //Seta o cursor na terceira coluna, segunda linha
        disp.print("Teste botao");      //Imprime o dia
        delay(5000);                    //Delay de 5 segundos exibindo a data
        disp.setCursor(0, 1);           //Seta o cursor na coluna 1, linha 2
        disp.print("                "); //"Apaga" a linha inteira
        limpaFuncoes();                 //Seta a função do botão P (DIREITA) para 0
    }
}

void telaTemperatura()
{
    disp.setCursor(2, 0); //Posiciona cursor na coluna 2, linha 1
    disp.print("Temperatura:");
    disp.setCursor(6, 1); //Posiciona cursor na coluna 1, linha 1
    disp.print(t);
}

void telaHumidadeSolo()
{
    disp.setCursor(1, 0); //Posiciona cursor na coluna 2, linha 1
    disp.print("Humidade solo:");
    disp.setCursor(6, 1); //Posiciona cursor na coluna 1, linha 1
    disp.print(humidade);
    disp.print(" %");
}

void telaUltimaIrrigacao()
{

    disp.setCursor(0, 0); //Posiciona cursor na coluna 1, linha 1
    disp.print("Ultima irrigacao");
    disp.setCursor(1, 1); //Posiciona cursor na coluna 1, linha 1
    disp.print(ultimaHoraIrrigado);
    disp.print(":");
    disp.print(ultimoMinutoIrrigado);
    disp.print(" - ");
    disp.print(ultimoDiaIrrigado);
    disp.print("/");
    disp.print(ultimoMesIrrigado);

    if (funcao_butSelect)
    {
        disp.clear();
        disp.setCursor(0, 0);
        disp.print("Ultima irrigacao");
        disp.setCursor(1, 1);
        disp.print("atualizada!");
        delay(5000);
        disp.clear();
        limpaFuncoes();
    }
}

void limpaFuncoes()
{
    funcao_butUp = 0;
    funcao_butDown = 0;
    funcao_butDireito = 0;
    funcao_butEsquerdo = 0;
    funcao_butSelect = 0;
}

void leTemperatura()
{

  h = dht.readHumidity();
  t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    //Serial.print("Umidade: ");
    //Serial.print(h);
    //Serial.print(" %t");
    //Serial.print("Temperatura: ");
    //Serial.print(t);
    //Serial.println(" *C");
  }
}

void leHumidadeSolo()
{
  humidadeAnalogico = analogRead(sensorHumidade);
  humidade = map(humidadeAnalogico, 1023, 0, 0, 100);
  //Serial.print("Humidade solo: ");
  //Serial.println(humidade);
}
