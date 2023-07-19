#ifndef GENERATOR_H
#define GENERATOR_H

#include "cpu.h"

Instruction* generateRandomInstructions(int);
Instruction* readInstructions(char*, int*);
Instruction *multiply(int , int );
Instruction *generateMultiplicationInstructions(int , int );
Instruction *fibo(int, int);
Instruction *exponentiation(int , int );
Instruction *division(int, int);

#endif // !GENERATOR_H