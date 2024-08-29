#pragma once
#ifndef SUDOKU_X_SUDOKU_H
#define SUDOKU_X_SUDOKU_H

#include <stdlib.h>
#include "../../SAT/include/SAT.h"
#include "../../OptSAT/include/OptSAT.h"

#define EMPTY 0

typedef struct Sudoku{
    int table[9][9];
}Sudoku;

typedef struct Hole{
    int x;
    int y;
}Hole;

Hole randomLocation();
void initSudoku(Sudoku& s);
bool las_vegas(int n);
void test2();

#endif //SUDOKU_X_SUDOKU_H
