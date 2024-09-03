#pragma once
#ifndef SUDOKU_OPTSAT_H
#define SUDOKU_OPTSAT_H

#include <stdio.h>
#include "../../SAT/include/SAT.h"

#define UNKNOWN 0

#define CONFLICT 1
#define UNSATISFIABLE 2
#define SATISFIABLE 3

typedef struct LiteralNums{
    int LiteralName;
    int Num;
    int PositiveNum;
    int NegativeNum;
}LiteralNums;

Answer OptDPLLSolution(Formular& formular);
void OptDPLL(Formular& formular, Answer& ans);
int chooseLiteral(Formular formular);
Answer OptDPLLSolution(Formular& formular);
void OptDPLL(Formular& formular, Answer& ans);

#endif //SUDOKU_OPTSAT_H
