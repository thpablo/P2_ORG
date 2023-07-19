#include "cpu.h"
#include "generator.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char**argv) {

    srand(1507);   // Inicializacao da semente para os numeros aleatorios.

    if (argc < 6) {
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
        if(argc != 8){
            printf("Quantidade invalida de argumentos\n");
            printf("Exemplo de execucao: ./exe multi 10 2 4 6 5 3 (5 * 3)\n");
        }
        numOperations[0] = atoi(argv[6]);
        numOperations[1] = atoi(argv[7]);
        memoriesSize[0]  = atoi(argv[2]);
        instructions = multiply(numOperations[0], numOperations[1]);
    } else if(strcmp(argv[1], "fibo") == 0){
        if(argc != 7){
            printf("Quantidade invalida de argumentos\n");
            printf("Exemplo de execucao: ./exe fibo 10 2 4 6 10 (fib de 10)\n");
        }
        numOperations[0] = atoi(argv[6]);
        memoriesSize[0]  = atoi(argv[2]);
        instructions = fibo(numOperations[0], 0);
    } else if(strcmp(argv[1], "expo") == 0){
        if(argc != 8){
            printf("Quantidade invalida de argumentos\n");
            printf("Exemplo de execucao: ./exe expo 10 2 4 6 2 4 (2^4)\n");
        }
        numOperations[0] = atoi(argv[6]);
        numOperations[1] = atoi(argv[7]);
        memoriesSize[0]  = atoi(argv[2]);
        instructions = exponentiation(numOperations[0], numOperations[1]);
    }
    else if(strcmp(argv[1], "div") == 0){
        if(argc != 8){
            printf("Quantidade invalida de argumentos\n");
            printf("Exemplo de execucao: ./exe div 10 2 4 6 8 2 (8/2)\n");
        }
        numOperations[0] = atoi(argv[6]);
        numOperations[1] = atoi(argv[7]);
        memoriesSize[0]  = atoi(argv[2]);
        instructions = division(numOperations[0], numOperations[1]);
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
