/*
 * OptSAT.h
 * Created by Yujie Zhou
 * date: 2024-9-4
 */
#pragma once
#ifndef SUDOKU_OPTSAT_H
#define SUDOKU_OPTSAT_H

#include <stdio.h>
#include "../../SAT/include/SAT.h"

#define UNKNOWN 0

//用于统计文字中相关指标的结构
typedef struct LiteralNums{
    int LiteralName;
    int Num;
    int PositiveNum;
    int NegativeNum;
}LiteralNums;

Answer OptDPLLSolution(Formular& formular);
void OptDPLL(Formular& formular, Answer& ans);
int chooseLiteral(Formular formular, bool& is_negative);
inline double valueMix(LiteralNums a);
inline int value(LiteralNums a);
#endif //SUDOKU_OPTSAT_H
