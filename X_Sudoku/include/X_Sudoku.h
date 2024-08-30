#pragma once
#ifndef SUDOKU_X_SUDOKU_H
#define SUDOKU_X_SUDOKU_H

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../../SAT/include/SAT.h"
#include "../../OptSAT/include/OptSAT.h"

#define EMPTY 0
#define MAX_HOLES 64 //根据文献数独最多只能挖64个洞
#define NORMAL 1    //普通数独
#define DIAGONAL 2  //对角线数独

typedef struct Hole{
    int x;
    int y;
}Hole;

typedef struct Sudoku{
    int ProblemTable[9][9];
    int SolutionTable[9][9];
    int numHoles;
    int numFilled;
    Hole holes[MAX_HOLES];
}Sudoku;

bool rulesExist(char* file);
inline int getLiteral(int row, int col, int num);
void writeRules(int type);
Hole randomLocation();
void initSudoku(Sudoku& s);
bool las_vegas(int n);
void test2();

#endif //SUDOKU_X_SUDOKU_H
