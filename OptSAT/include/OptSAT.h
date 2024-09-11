/*
 * SAT.cpp
 * Created by Yujie Zhou
 * date: 2024-9-4
 */
#pragma once
#ifndef SUDOKU_OPTSAT_H
#define SUDOKU_OPTSAT_H

#include <stdio.h>
#include "../../SAT/include/SAT.h"

#define UNKNOWN 0


typedef struct LiteralNums{
    int LiteralName;
    int Num;
    int PositiveNum;
    int NegativeNum;
}LiteralNums;

Answer OptDPLLSolution(Formular& formular);
void OptDPLL(Formular& formular, Answer& ans);
int chooseLiteral(Formular formular, bool& is_negative);
void OptDPLL(Formular& formular, Answer& ans);
inline double valueMix(LiteralNums a);
#endif //SUDOKU_OPTSAT_H
