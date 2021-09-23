// include the library code:
#include <LiquidCrystal.h>

// Blibioteca sensor temperatura e humidade
#include "DHT.h"

// Biblioteca módulo Real Time Clock
#include <DS3231.h>

// pino que estamos conectando o sensor temperatura/humidade
#define DHTPIN A1

// Tipo do sensor de temperatura e humidade (DHT 11)
#define DHTTYPE DHT11

// Pino do sensor de humidade
#define sensorHumidade A4

// Pino entrada analógica dos botões
#define Botoes A0

// Variaveis dos horários
byte valorHoras = 1;
byte valorMinutos = 1;

// Objeto DTH sensor temperatura
DHT dht(DHTPIN, DHTTYPE);

// Objeto modulo Real Time Clock
DS3231 rtc; //objeto do tipo DS3231

// Objeto data e hora
RTCDateTime dataehora; //Criação do objeto do tipo RTCDateTime

// --- Protótipo das Funções Auxiliares ---
void changeMenu();         //Função para modificar o menu atual
void dispMenu();           //Função para mostrar o menu atual
void lerBotoes();          //Função que le o estado dos botoes
void leTemperaturaAtual(); //Funcao que le a temperatura atual

// --- Protótipo das Funções das telas ---
void tela_inicial();        //Função da tela inicial, data e hora atuais
void telaTemperatura();     //Função horário programável 1
void telaHumidadeSolo();    //Função horário programável 2
void telaUltimaIrrigacao(); //Função horário programável 3

// --- Variáveis Globais ---
int menu = 1; //Variável para selecionar o menu

boolean estado_butUP, estado_butDown, estado_butDireito, estado_butEsquerdo, estado_butSelect; //Flags para armazenar o estado dos botões

boolean funcao_butUp, funcao_butDown, funcao_butDireito, funcao_butEsquerdo, funcao_butSelect; //Flags para armazenar a função dos botões

// --- Variáveis das escolhas dos horários
byte ultimoDiaIrrigado = 0;
byte ultimoMesIrrigado = 0;
byte ultimaHoraIrrigado = 0;
byte ultimoMinutoIrrigado = 0;

// Define os botoes do display LCD
LiquidCrystal disp(8, 9, 4, 5, 6, 7);

// Variaveis para guardar a temperatura e humidade do ambiente
float humidadeAmbiente;
float temperaturaAmbiente;

// --- Variavel que guarda a humidade do solo
int humidadeAnalogicoSolo;

// Variavel que guarda a porcentagem de humidade do solo
int humidadeSolo;

// Objeto hora e data
RTCDateTime dataehora;

void setup()
{
    // Define pino A0 como INPUT
    pinMode(Botoes, INPUT);

    // Define o pino A4 como INPUT
    pinMode(sensorHumidade, INPUT);

    //Inicializa o RTC
    rtc.begin();

    // Inicia o display LCD
    disp.begin(16, 2);

    // Inicia serial
    Serial.begin(9600);

    // Inicia o DHT
    dht.begin();

    estado_butUP = 0x00;       //limpa flag do botão Up
    estado_butDown = 0x00;     //limpa flag do botão Down
    estado_butDireito = 0x00;  //limpa flag do botão P
    estado_butEsquerdo = 0x00; //limpa flag do botão M

    // --- Definir a hora, dia da semana e o ano do RTC --- EXECUTAR APENAS UMA VEZ
    //rtc.setDOW(WEDNESDAY); //Setar o dia da semana em INGLES
    //rtc.setTime(19, 22, 0); //Seta o horário. Ex.: para 12:00:00 (24hr format)
    //rtc.setDate(26, 10, 2017); //Seta a data. Ex.: 20 de Agosto de 2017

    //rtc.setDateTime(__DATE__, __TIME__);
}

void loop() // Laco infinito
{
    dataehora = rtc.getDateTime(); // Obtem a data e a hora e 'escreve' na variável
    leTemperaturaAtual();          // Le a temperatura atual
    leHumidadeSolo();              // Le a humidade do solo
    lerBotoes();                   //Verifica o estado dos botões
    changeMenu();                  //Muda a tela conforme a selecionada
    dispMenu();                    // Exibe o item do menu

    //Serial.println(analogRead(Botoes)); // Printa no serial o valor analogico dos botoes
}

// --- Desenvolvimento das Funções Auxiliares ---
void lerBotoes()
{
    // Le o valor dos botoes
    int valorbotao = analogRead(Botoes);

    if (valorbotao > 1008 && valorbotao < 1020)
        estado_butDireito = 0x01;             //Botão DIREITO pressionado? Seta flag
    if (valorbotao < 50 && estado_butDireito) //Botão DIREITO solto e flag setada?
    {                                         //Se for sim...
        estado_butDireito = 0;                //Limpa flag
        funcao_butDireito = 1;                //Ativa função Botao DIREITO
    }
    else if (valorbotao > 340 && valorbotao < 372)
        estado_butEsquerdo = 0x01;             //Botão ESQUERDO pressionado? Seta flag
    if (valorbotao < 50 && estado_butEsquerdo) //Botão ESQUERDO solto e flag setada?                         // BOTÃO LADO ESQUERDO
    {                                          //Se for sim...
        estado_butEsquerdo = 0;                //Limpa flag
        funcao_butEsquerdo = 1;                //Ativa função Botao ESQUERDO
    }
    else if (valorbotao > 500 && valorbotao < 540)
        estado_butSelect = 0x01;             //Botão SELECT pressionado? Seta flag
    if (valorbotao < 50 && estado_butSelect) //Botão SELECT solto e flag setada?                         // BOTÃO SELECT
    {                                        //Se for sim...
        estado_butSelect = 0;                //Limpa flag
        funcao_butSelect = 1;                //Ativa função Botao SELECT
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

        break;             //break
    case 0x02:             //Caso 4
        telaTemperatura(); //Chama função para o definir a quantidade

        break;              //break
    case 0x03:              //Caso 4
        telaHumidadeSolo(); //Chama função para o controle dos minutos

        break;                 //break
    case 0x04:                 //Caso 4
        telaUltimaIrrigacao(); //Chama função para o controle dos minutos

        break; //break
    }          //end switch menu
} //end dispMenu

void tela_inicial()
{
    disp.setCursor(0, 0); //Posiciona cursor na coluna 2, linha 1
    disp.print("Joana: a planta.");
    disp.setCursor(4, 1);       //Posiciona cursor na coluna 1, linha 2
    disp.print(dataehora.hour); //Informa as horas atraves do RTC
    disp.print(":");
    disp.print(dataehora.minute);
    disp.print("h");

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

void limpaFuncoes() // Funcao que limpa as variaveis de funcao
{
    funcao_butUp = 0;
    funcao_butDown = 0;
    funcao_butDireito = 0;
    funcao_butEsquerdo = 0;
    funcao_butSelect = 0;
}

void leTemperaturaAtual() // Funcao que le a temperatura atual
{

    humidadeAmbiente = dht.readHumidity();
    temperaturaAmbiente = dht.readTemperature();

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
    humidadeAnalogicoSolo = analogRead(sensorHumidade);
    humidadeSolo = map(humidadeAnalogicoSolo, 1023, 0, 0, 100);
    //Serial.print("Humidade solo: ");
    //Serial.println(humidade);
}
