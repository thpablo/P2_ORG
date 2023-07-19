#include "cpu.h"
#include "generator.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Instruction *multiply(int , int );
Instruction *generateMultiplicationInstructions(int , int );

int main(int argc, char**argv) {

    srand(1507);   // Inicializacao da semente para os numeros aleatorios.

    if (argc != 6) {
        printf("Numero de argumentos invalidos! Sao 5.\n");
        printf("Linha de execucao: ./exe TIPO_INSTRUCAO [TAMANHO_RAM|ARQUIVO_DE_INSTRUCOES] TAMANHO_L1 TAMANHO_L2 TAMANHO_L3\n");
        printf("\tExemplo 1 de execucao: ./exe random 10 2 4\n");
        printf("\tExemplo 2 de execucao: ./exe file arquivo_de_instrucoes.txt 2 4\n");
        return 0;
    }

    int memoriesSize[4];
    int numOperations[2];
    Machine machine;
    Instruction *instructions;

    memoriesSize[1] = atoi(argv[3]);
    memoriesSize[2] = atoi(argv[4]);
    memoriesSize[3] = atoi(argv[5]);
    if (strcmp(argv[1], "random") == 0) {
        memoriesSize[0] = atoi(argv[2]);
        instructions = generateRandomInstructions(memoriesSize[0]);
    } else if (strcmp(argv[1], "file") == 0) {
        instructions = readInstructions(argv[2], memoriesSize);
    } else if(strcmp(argv[1], "multi") == 0){
        memoriesSize[0] = atoi(argv[2]);
        instructions = multiply(200, 15);
    }

    else {
        printf("Invalid option.\n");
        return 0;
    }
    
    printf("Starting machine...\n");
    start(&machine, instructions, memoriesSize);
    if (memoriesSize[0] <= 10)
        printMemories(&machine);
    run(&machine);
    if (memoriesSize[0] <= 10)
        printMemories(&machine);
    stop(&machine);
    printf("Stopping machine...\n");
    return 0;
}


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