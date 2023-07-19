#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

Instruction* generateRandomInstructions(int ramSize) {
    // 01|22|13|45 => this is an instruction
    // 02|33|12|01 => this is another instruction
            
    // 0 => opcode => move
    // 1 => opcode => sum
    // 2 => opcode => subtract
    //-1 => halt
    
    int n = 10;

    Instruction* instructions = (Instruction*) malloc((n + 1) * sizeof(Instruction));

    for (int i=0; i<n; i++) {
        instructions[i].add1.block = rand() % ramSize;
        instructions[i].add1.word = rand() % WORDS_SIZE;
        instructions[i].add2.block = rand() % ramSize;
        instructions[i].add2.word = rand() % WORDS_SIZE;
        instructions[i].add3.block = rand() % ramSize;
        instructions[i].add3.word = rand() % WORDS_SIZE;
        instructions[i].opcode = rand() % 3;
    }
    instructions[n].opcode = -1;
    return instructions;
}

Instruction* readInstructions(char* fileName, int* memoriesSize) {
    Instruction* instructions = NULL;

    printf("FILE -> %s\n", fileName);
    FILE* file = fopen(fileName, "r"); // Open file in read mode
    
    if (file == NULL) {
        printf("Arquivo nao pode ser aberto.\n");
        exit(1);
    }

    int n, i = 0;
    fscanf(file, "%d %d", &n, &memoriesSize[0]);
    instructions = (Instruction*) malloc(n * sizeof(Instruction));
    while (i < n) {
        fscanf(file, "%d %d %d %d %d %d %d", 
            &instructions[i].opcode, 
            &instructions[i].add1.block, &instructions[i].add1.word,
            &instructions[i].add2.block, &instructions[i].add2.word,
            &instructions[i].add3.block, &instructions[i].add3.word);
        i++;
    }
    fclose(file); // Close file

    return instructions;
}

/* Operacoes */
/* Multiplicacao */
Instruction *multiply(int n1, int n2){
    Instruction *instructions = (Instruction *)malloc((n2 + 4) * sizeof(Instruction)); /*o tamanho dele será igual ao n2 + 4 operações manuais*/
    Instruction *in = generateMultiplicationInstructions(0, n2);
    
    Instruction inst;
    /*salvando o valor n1(informado) na ram 0*/

    inst.opcode = 0;        //OPCode de mover
    inst.add1.block = n1;   //add1.block = valor que vai ser salvo
    inst.add2.block = 1;    //add2.block =  Bloco onde vai ser salvo n1
    inst.add2.word = 0;     //add2.word =  Palavra onde vai ser salvo n1
    instructions[0] = inst;

    /*salvando o valor n2 (informado)na ram 1*/
    inst.opcode = 0;
    inst.add1.block = n2;
    inst.add2.block = 1;    //add2.block =  Bloco onde vai ser salvo n1
    inst.add2.word = 1;     //add2.word =  Palavra onde vai ser salvo n1
    instructions[1] = inst;
    
    /*inicializando a ram 2, local onde será armazenado o valor dos calculos*/
    inst.opcode = 0;
    inst.add1.block = 0;
    inst.add2.block = 1;    //add2.block =  Bloco onde vai ser salvo n1
    inst.add2.word = 2;     //add2.word =  Palavra onde vai ser salvo n1
    instructions[2] = inst;

    /*criando o HALT*/
    inst.opcode = -1;
    inst.add1.block = -1; inst.add1.word = -1;
    inst.add2.block = -1; inst.add2.word = -1;
    inst.add3.block = -1; inst.add3.word = -1;

    instructions[n2 + 3] = inst; // Colocando ultima posicao

    int indexK = 0;
    /*concatenando vetor*/
    for (int k = 3; k < (n2 + 3); k++)
    {
        instructions[k] = in[indexK];
    }

    return instructions;
}

Instruction *generateMultiplicationInstructions(int address, int n)
{
    /*Alocação foi setada para o tamanho do n, pois é n é o tamanho das instruções realizadas pelo
      FOR e o +4 é pq existem 4 instruções antes do FOR*/
    Instruction *instructions = (Instruction *)malloc((n) * sizeof(Instruction));
    Instruction inst;

    for (int i = 0; i < n; i++)
    {
        /*A instrução pela os valores dentro do endereço informado + 2 e soma colocando o resulado dentro do campo 3*/
        inst.opcode = 1;
        inst.add1.block = 1;
        inst.add1.word = 2;

        inst.add2.block = 1;
        inst.add2.word = 0;

        inst.add3.block = 1;
        inst.add3.word = 2;
        instructions[i] = inst;

    }

    return instructions;
}

Instruction *division(int dividendo, int divisor)
{

    /* calculando o quociente, utilizado na parte de subtração sucessiva, posteriomente*/
    int quociente = 0;
    int calcDividendo = dividendo;

    while (calcDividendo >= divisor)
    {
        calcDividendo -= divisor;
        quociente++;
    }

    Instruction *instructions = (Instruction *)malloc((divisor + 7) * sizeof(Instruction));

    Instruction inst;
    /*instruções manuais*/
    inst.opcode = 0;        //OPCode de mover
    inst.add1.block = dividendo;   //add1.block = valor que vai ser salvo
    inst.add2.block = 1;    //add2.block =  Bloco onde vai ser salvo n1
    inst.add2.word = 0;     //add2.word =  Palavra onde vai ser salvo n1
    instructions[0] = inst;

    inst.opcode = 0;                //OPCode de mover
    inst.add1.block = quociente;    //add1.block = valor que vai ser salvo
    inst.add2.block = 1;            //add2.block =  Bloco onde vai ser salvo n1
    inst.add2.word = 1;             //add2.word =  Palavra onde vai ser salvo n1
    instructions[1] = inst;

    /*for que realiza as subrtrações, ou seja, divisões*/
    for (int i = 0; i < divisor; i++)
    {

        inst.opcode = 2;
        inst.add1.block = 1;
        inst.add1.word = 0;

        inst.add2.block = 1;
        inst.add2.word = 1;

        inst.add3.block = 1;
        inst.add3.word = 0;

        instructions[i + 2] = inst;
    }

    /*colocando umas instruções manuais para exibição do resultado na tela*/
    inst.opcode = 0;
    inst.add1.block = dividendo;
    inst.add2.block = 1;
    inst.add2.word = 0;
    instructions[divisor + 2] = inst;

    inst.opcode = 0;
    inst.add1.block = 0;
    inst.add2.block = 1;
    inst.add2.word = 2;
    instructions[divisor + 3] = inst;

    inst.opcode = 1;
    inst.add1.block = 1;
    inst.add1.word = 1;

    inst.add2.block = 1;
    inst.add2.word = 2;

    inst.add3.block = 1;
    inst.add3.word = 2;

    instructions[divisor + 4] = inst;

    inst.opcode = 0;
    inst.add1.block = divisor;
    inst.add2.block = 1;
    inst.add2.word = 1;
    instructions[divisor + 5] = inst;

    /*condição de finalizaçao da máquina (HALT)*/
    inst.opcode = -1;
    inst.add1.block = -1;
    inst.add1.word = -1;
    inst.add2.block = -1;
    inst.add2.word = -1;
    inst.add3.block = -1;
    inst.add3.word = -1;
    // add na ultima posição do vetor
    instructions[divisor + 6] = inst;

    return instructions;
}

Instruction* fibo(int termos, int n){
    /*1: Instrucao -> Levar valor num1 para memória ram */
    /*2: Instrucao -> Reservar valor antigo para memória ram */
    /*3: Instrucao -> Soma Fibonacci */
    /*4: Finalizar máquina */

     /* quantTermos é duplicado pois a cada soma, 
     há uma instrucao de mover os valores
     Duplicando a quantidade de operacoes. 
     É somado 4 para instruções de Mover Dados/Finalizar operação. 
     */
    //Termos negativos
    if(termos < 1)
        termos = 1;

    int quantTermos = 1 + (termos * 2);
    Instruction* instructions = (Instruction*) malloc(quantTermos * sizeof(Instruction)); //Mudar quantidade de instrucoes


    // Valor Resposta
    instructions[n].opcode      = 0;
    instructions[n].add1.block  = 0;   //Valor inicial -> 0
    instructions[n].add2.block  = 1;  //Bloco para valor 
    instructions[n].add2.word   = 0;


    //Valor Fib Termo antigo
    instructions[n + 1].opcode      = 0;
    instructions[n + 1].add1.block  = 1; //Antigo valor = 1;
    instructions[n + 1].add2.block  = 1; //Bloco para valor antigo;
    instructions[n + 1].add2.word   = 1; //Palavra para o valor antigo

    
    for (int i = n + 2; i <  quantTermos - 2; i += 2){ //Pula-se dois pois há as instrucoes de soma e instrucoes de subtracao
        //SOMAR OldValue com Res;
        instructions[i].opcode = 1;
        instructions[i].add1.block  = 1; //Soma valor antigo
        instructions[i].add1.word   = 1;
        instructions[i].add2.block  = 1; 
        instructions[i].add2.word   = 0;

        instructions[i].add3.block  = 1; 
        instructions[i].add3.word   = 1; //Armazena em Res

        //SUBTRAIR Res com OldValue, resultado armazenado em OldValue;
        instructions[i + 1].opcode = 2;
        instructions[i + 1].add1.block  = 1; 
        instructions[i + 1].add1.word   = 0;

        instructions[i + 1].add2.block  = 1; 
        instructions[i + 1].add2.word   = 1;

        instructions[i + 1].add3.block  = 1; 
        instructions[i + 1].add3.word   = 0;
    }

    instructions[quantTermos - 1].opcode = -1;
    instructions[quantTermos - 1].add1.block = -1;
    instructions[quantTermos - 1].add1.word = -1;
    instructions[quantTermos - 1].add2.block = -1;
    instructions[quantTermos - 1].add2.word = -1;
    instructions[quantTermos - 1].add3.block = -1;
    instructions[quantTermos - 1].add3.word = -1; 

   return instructions;
}

Instruction *exponentiation(int n1, int n2) /*Função que realiza calculo da exponenciação*/
{
    /*Esse calculo n1 * n2 me retornara a quantidade de interações de multiplicação e n2 * 3 é porque existem 3 instruções manuais dentro do laço q vai ate n2 vzs + 5 para instruções manuais*/
    Instruction *instructions = (Instruction *)malloc(((n1 * n2) + (n2 * 3) + 5) * sizeof(Instruction)); // o 5 é pra corrigir caso a entrada seja 0 no n2

    int count = 1;
    int index = 3;

    Instruction inst;
    /*salvando o valor n1(informado) no bloco 1 palavra 0*/
    inst.opcode = 0;
    inst.add1.block = n1;
    inst.add2.block = 1;
    inst.add2.word = 0;
    instructions[0] = inst;

    /*salvando o valor n1(informado) no bloco 1 palavra 1*/
    inst.opcode = 0;
    inst.add1.block = n2;
    inst.add2.block = 1;
    inst.add2.word = 1;
    instructions[1] = inst;
    /*inicializando a ram 2, local onde será armazenado o valor dos calculos*/

    if (n2 == 0)
    {
        inst.opcode = 0;
        inst.add1.block = 1;
        inst.add2.block = 1;
        inst.add2.word = 2;
        instructions[2] = inst;

    /* HALT */
        inst.opcode = -1;
        inst.add1.block = -1;
        inst.add1.word = -1;
        inst.add2.block = -1;
        inst.add2.word = -1;
        inst.add3.block = -1;
        inst.add3.word = -1;

        instructions[3] = inst;
        return instructions;
    }

    if (n2 == 1)
    { /*Instruções para caso o expoente seja 1, retornar o propio valor 1*/
        inst.opcode = 0;
        inst.add1.block = n2;
        inst.add2.block = 1;
        inst.add2.word = 2;
        instructions[2] = inst;

        inst.opcode = -1;
        inst.add1.block = -1;
        inst.add1.word = -1;
        inst.add2.block = -1;
        inst.add2.word = -1;
        inst.add3.block = -1;
        inst.add3.word = -1;

        return instructions;
    }
    /*inicializando com uma soma manual*/
    inst.opcode = 0;
    inst.add1.block = 0;
    inst.add2.block = 1;
    inst.add2.word = 2;
    instructions[2] = inst;

    /*criando o HALT*/
    inst.opcode = -1;
    inst.add1.block = -1;
    inst.add1.word = -1;
    inst.add2.block = -1;
    inst.add2.word = -1;
    inst.add3.block = -1;
    inst.add3.word = -1;
    instructions[((n1 * n2) + (n2 * 3) - 1)] = inst; /*colocando HALT no final do array*/

    for (int i = 1; i < n2; i++)
    {
        Instruction *ins = generateMultiplicationInstructions(0, n1);
        int indexK = 0;

        /*concatenacao de vetores*/

        for (int k = index; k < (index + n1); k++)
        {
            instructions[k] = ins[indexK];
        }
        // zzerando a ram um e depois atribuindo valor a ela
        inst.opcode = 0;
        inst.add1.block = 0;
        inst.add2.block = 1;
        inst.add2.word = 0; 
        instructions[index + n1] = inst;

        //Soma valor do resultado no antigo valor de n1
        inst.opcode = 1;
        inst.add1.block = 1;
        inst.add1.word = 0;

        inst.add2.block = 1;
        inst.add2.word = 2;

        inst.add3.block = 1;
        inst.add3.word = 0; 
        instructions[index + n1 + 1] = inst;

        // para não zerar a RAM 2 quando estiver na ultima execucao
        if (i != (n2 - 1)){
            inst.opcode = 0;
            inst.add1.block = 0;
            inst.add2.block = 1;
            inst.add2.word = 2; 
            instructions[index + n1 + 2] = inst;
        }
        index += n1 + 3; // pulado 3 para n alterar o valor alocado manualmente

        count++;
    }
    /*colocando o valor n1 na ram 0 para exibição na telas*/
    inst.opcode = 0;
    inst.add1.block = n1;
    inst.add2.block = 1;
    inst.add2.word = 0; 
    instructions[((n1 * n2) + (n2 * 3) - 2)] = inst;

    return instructions;
}

