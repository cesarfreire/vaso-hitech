// include the library code:
#include <LiquidCrystal.h>

#define Botoes A0
#define Motor 3
#define luzA 13
#define luzB 12

// --- Protótipo das Funções Auxiliares ---
void changeMenu(); //Função para modificar o menu atual
void dispMenu(); //Função para mostrar o menu atual
void lerbotoes(); //Função que le o estado dos botoes
void girarAgora();
void tela_inicial(); //Função do menu1, data e hora atuais
void tela_horario_1(); //Função horário programável 1
void tela_definir_qtd(); //Função para definir a quantidade de ração a ser dispensada;
void piscarLuzes(int segundos);

// --- Variáveis Globais ---
int menu = 1; //Variável para selecionar o menu

boolean estado_butUP, estado_butDown, estado_butDireito,
estado_butEsquerdo, estado_butSelect; //Flags para armazenar o estado dos botões

boolean funcao_butUp, funcao_butDown, funcao_butDireito,
funcao_butEsquerdo, funcao_butSelect; //Flags para armazenar a função dos botões

// --- Variáveis das escolhas dos horários

byte valorHora1= 1;
byte valorHora2= 1;
byte valorHora3= 1;
byte valorMinutos1, valorMinutos2, valorMinutos3 = 0;          //Variável com valor inicial dos minutos

byte numeroVoltas = 1;

// --- Variaveis gerais ---
unsigned int delayMotor = 3000;
unsigned int tempo1 = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal disp(8, 9, 4, 5, 6, 7);

void setup() {
    pinMode(Botoes, INPUT);
    pinMode(Motor, OUTPUT);
    pinMode(luzA, OUTPUT);
    pinMode(luzB, OUTPUT);
    digitalWrite(luzA, LOW);
    digitalWrite(luzB, LOW);
  
    // set up the LCD's number of columns and rows:
    disp.begin(16, 2);
    // Print a message to the LCD.
    Serial.begin(9600);

    estado_butUP = 0x00; //limpa flag do botão Up
    estado_butDown = 0x00; //limpa flag do botão Down
    estado_butDireito = 0x00; //limpa flag do botão P
    estado_butEsquerdo = 0x00; //limpa flag do botão M
}

void loop() {

    lerbotoes(); //Verifica o estado dos botões
    changeMenu(); //Muda a tela conforme a selecionada
    dispMenu();
    Serial.println(analogRead(Botoes));

}

// --- Desenvolvimento das Funções Auxiliares ---
void lerbotoes() {
    int valorbotao = analogRead(Botoes);
    if (valorbotao > 510 && valorbotao < 515) estado_butUP = 1; //Botão CIMA pressionado? Seta flag
    if (valorbotao == 0 && estado_butUP) //Botão CIMA solto e flag setada
    { //Sim...
        estado_butUP = 0; //Seta a flag em 0, não apertado
        funcao_butUp = 1; //Ativa função Botao CIMA
    } else if (valorbotao > 255 && valorbotao < 257) estado_butDown = 1; //Botão BAIXO pressionado? Seta flag
    if (valorbotao == 0 && estado_butDown) //Botão BAIXO solto e flag setada?
    { //Se for sim...
        estado_butDown = 0; //Limpa flag
        funcao_butDown = 1; //Ativa função Botao BAIXO
    } else if (valorbotao > 340 && valorbotao < 345) estado_butDireito = 0x01; //Botão P pressionado? Seta flag
    if (valorbotao == 0 && estado_butDireito) //Botão P solto e flag setada?                         // BOTÃO LADO DIREITO
    { //Se for sim...
        estado_butDireito = 0; //Limpa flag
        funcao_butDireito = 1; //Ativa função Botao p
    } else if (valorbotao > 200 && valorbotao < 210) estado_butEsquerdo = 0x01; //Botão M pressionado? Seta flag
    if (valorbotao == 0 && estado_butEsquerdo) //Botão M solto e flag setada?                         // BOTÃO LADO ESQUERDO
    { //Se for sim...
        estado_butEsquerdo = 0; //Limpa flag
        funcao_butEsquerdo = 1; //Ativa função Botao M  
    } else if (valorbotao > 1020) estado_butSelect = 0x01; //Botão M pressionado? Seta flag
    if (valorbotao == 0 && estado_butSelect) //Botão M solto e flag setada?                         // BOTÃO SELECT
    { //Se for sim...
        estado_butSelect = 0; //Limpa flag
        funcao_butSelect = 1; //Ativa função Botao M  
    }

} // end lerbotoes

void changeMenu() //Função para modificar o menu atual
{

    if (funcao_butUp) { //Se a função do botão CIMA foi ATIVA
        disp.clear(); //Limpa display
        menu++; //Incrementa o menu

        if (menu > 0x06) menu = 0x01; //Se menu maior que 6, volta a ser 1
        limpaFuncoes(); //Limpa função CIMA
    } //end butUp

    if (funcao_butDown) { //Se a função do botão BAIXO foi ATIVA                                                                 //Sim...
        disp.clear(); //Limpa display
        menu--; //Decrementa o menu

        if (menu < 0x01) menu = 0x06; //Se menu menor que 1, volta a ser 6
        limpaFuncoes(); //Limpa função BAIXO
    } //end butDown   

} //end changeMenu

void dispMenu() //Mostra o menu atual
{
    switch (menu) //Controle da variável menu
    {
    case 0x01: //Caso 1
        tela_inicial(); //Chama a função da marca e da hora

        break; //break
    case 0x02: //Caso 4
        tela_horario_1(); //Chama função para o definir a quantidade

        break; //break
    case 0x03: //Caso 4
        tela_horario_2(); //Chama função para o controle dos minutos

        break; //break
    case 0x04: //Caso 4
        tela_horario_3(); //Chama função para o controle dos minutos

        break; //break
    case 0x05: //Caso 4
        tela_definir_qtd();

        break; //break
    case 0x06:
        girarAgora(); //Chama a função para girar o motor agora

        break;
    } //end switch menu
} //end dispMenu

void tela_inicial() {
    disp.setCursor(2, 0); //Posiciona cursor na coluna 2, linha 1
    disp.print("Cesar Freire");
    disp.setCursor(4, 1); //Posiciona cursor na coluna 2, linha 2
    disp.print("11:21 hrs");

    if (funcao_butDireito) //Se a função do botão P (DIREITA) for acionada
    {
        disp.setCursor(0, 1); //Seta o cursor na segunda linha
        disp.print("                "); //"Apaga" a linha inteira
        disp.setCursor(2, 1); //Seta o cursor na terceira coluna, segunda linha
        disp.print("Teste botao"); //Imprime o dia
        delay(5000); //Delay de 5 segundos exibindo a data
        disp.setCursor(0, 1); //Seta o cursor na coluna 1, linha 2
        disp.print("                "); //"Apaga" a linha inteira
        limpaFuncoes(); //Seta a função do botão P (DIREITA) para 0
    }
}

void tela_horario_1() {

    disp.setCursor(3, 0); //Posiciona cursor na coluna 1, linha 1
    disp.print("Horario 1:");
    disp.setCursor(1, 1); //Posiciona cursor na coluna 1, linha 1
    disp.print(valorHora1);
    disp.print(" Hrs");
    disp.setCursor(9,1);
    disp.print(valorMinutos1);
    disp.print(" min");

    if(funcao_butDireito){
        valorMinutos1 = valorMinutos1 + 10;
        limpaFuncoes();
    }

    if(funcao_butEsquerdo){
        valorHora1++;
        limpaFuncoes();
    }

    if(valorHora1 > 23){
        valorHora1 = 1;
    }
    if(valorHora1 < 1){
        valorHora1 = 23;
    }
    if(valorMinutos1 > 60){
        valorMinutos1 = 0;
    }
    if (valorMinutos1 < 0){
        valorMinutos1 = 59;
    }
    if(valorMinutos1 == 60){
        valorMinutos1 = 59;
    }

    if (funcao_butSelect){
        disp.clear();
        disp.setCursor(3,0);
        disp.print("Horario 1");
        disp.setCursor(2,1);
        disp.print("Habilitado!");
        piscarLuzes(3);
        disp.clear();
        limpaFuncoes();
    }
}

void tela_horario_2() {

    disp.setCursor(3, 0); //Posiciona cursor na coluna 1, linha 1
    disp.print("Horario 2:");
    disp.setCursor(1, 1); //Posiciona cursor na coluna 1, linha 1
    disp.print(valorHora2);
    disp.print(" Hrs");
    disp.setCursor(9,1);
    disp.print(valorMinutos2);
    disp.print(" min");

    if(funcao_butDireito){
        valorMinutos2 = valorMinutos2 + 10;
        limpaFuncoes();
    }

    if(funcao_butEsquerdo){
        valorHora2++;
        limpaFuncoes();
    }

    if(valorHora2 > 23){
        valorHora2 = 1;
    }
    if(valorHora2 < 1){
        valorHora2 = 23;
    }
    if(valorMinutos2 > 60){
        valorMinutos2 = 0;
    }
    if (valorMinutos2 < 0){
        valorMinutos2 = 59;
    }
    if(valorMinutos2 == 60){
        valorMinutos2 = 59;
    }

    if (funcao_butSelect){
        disp.clear();
        disp.setCursor(3,0);
        disp.print("Horario 2");
        disp.setCursor(2,1);
        disp.print("Habilitado!");
        piscarLuzes(3);
        disp.clear();
        limpaFuncoes();
    }
}

void tela_horario_3() {

    disp.setCursor(3, 0); //Posiciona cursor na coluna 1, linha 1
    disp.print("Horario 3:");
    disp.setCursor(1, 1); //Posiciona cursor na coluna 1, linha 1
    disp.print(valorHora3);
    disp.print(" Hrs");
    disp.setCursor(9,1);
    disp.print(valorMinutos3);
    disp.print(" min");

    if(funcao_butDireito){
        valorMinutos3 = valorMinutos3 + 10;
        limpaFuncoes();
    }

    if(funcao_butEsquerdo){
        valorHora3++;
        limpaFuncoes();
    }

    if(valorHora3 > 23){
        valorHora3 = 1;
    }
    if(valorHora3 < 1){
        valorHora3 = 23;
    }
    if(valorMinutos3 > 60){
        valorMinutos3 = 0;
    }
    if (valorMinutos3 < 0){
        valorMinutos3 = 59;
    }
    if(valorMinutos3 == 60){
        valorMinutos3 = 59;
    }

    if (funcao_butSelect){
        disp.clear();
        disp.setCursor(3,0);
        disp.print("Horario 3");
        disp.setCursor(2,1);
        disp.print("Habilitado!");
        piscarLuzes(3);
        disp.clear();
        limpaFuncoes();
    }
}

void tela_definir_qtd(){

    disp.setCursor(0,0);                                 //Posiciona cursor na coluna 1, linha 1
    disp.print("Selec. o numero");                       //Imprime o título
    disp.setCursor(0,1);                                 //Posiciona cursor na coluna 0, linha 2
    disp.print(" de voltas = "); 
    disp.print(numeroVoltas);                               //Imprime o valor da variável da quantidade

    if(funcao_butDireito){
        numeroVoltas++;
        limpaFuncoes();
    }
    if(funcao_butEsquerdo){
        numeroVoltas--;
        limpaFuncoes();
    }

    if(numeroVoltas > 5){
        numeroVoltas = 1;
    }
    if(numeroVoltas < 1){
        numeroVoltas = 5;
    }

    if(funcao_butSelect){
        disp.clear();
        disp.setCursor(3,0);
        disp.print("Quantidade");
        disp.setCursor(5,1);
        disp.print("salva!");                       //Imprime a mensagem
        piscarLuzes(3);
        
        limpaFuncoes();
    }
}

void girarAgora(){
    disp.setCursor(0,0);
    disp.print("Alimentar agora:");
    disp.setCursor(0,1);
    disp.print("Aperte p/ girar");

    if(funcao_butSelect){
        disp.clear();
        disp.setCursor(3,0);
        disp.print("Aguarde...");
        disp.setCursor(1,1);
        disp.print("Alimentando...");
    //piscarLuzes();
        //Função rotacionar o motor..
        rotacionarMotor();

        disp.clear();
        disp.setCursor(3,1);
        disp.print("Concluido!");                           //Imprime a mensagem de conclusão do processo
        delay(3000);

        limpaFuncoes();
    }
}


void rotacionarMotor(){
    digitalWrite(Motor, HIGH);
    piscarLuzes(4);
    digitalWrite(Motor, LOW);
}

void limpaFuncoes(){
  funcao_butUp = 0;
  funcao_butDown = 0;
  funcao_butDireito = 0;
  funcao_butEsquerdo = 0;
  funcao_butSelect = 0;
}

void piscarLuzes(int segundos){
  
  for(int c = 0; c < segundos; c++){
    digitalWrite(luzA, HIGH);
  delay(250);
    digitalWrite(luzB, HIGH);
    delay(250);
    digitalWrite(luzA, LOW);
    delay(250);
    digitalWrite(luzB, LOW);
    delay(250);
  }
  
}