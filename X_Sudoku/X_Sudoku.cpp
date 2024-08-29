#include <stdio.h>
#include "include/X_Sudoku.h"

Hole randomLocation(){
    Hole h;
    h.x = rand() % 9;
    h.y = rand() % 9;
    return h;
}

void initSudoku(Sudoku& s){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            s.table[i][j] = EMPTY;
        }
    }
}

void test2(){
    printf("---X_Sudo---\n");
}