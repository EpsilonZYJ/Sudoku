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

#define HARD 5
#define EASY 6

typedef struct Hole{
    int x;
    int y;
}Hole, Position;

typedef struct PosNum{
    int x;
    int y;
    int num;
}PosNum;

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
bool checkOneSudokuAnswer(Answer ans);
void test2();
bool SolveSudokuTable(int table[9][9], int type, void (*pDPLL)(Formular& formular, Answer& ans));
void readSudokuTable(Sudoku& sudoku, int LineNum);
void printTable(int table[9][9]);
void tableFormularAdd(Formular& formular, int table[9][9]);
void swap(int& a, int& b);
Answer encodeTable(int table[9][9]);
void DigHole(Sudoku& sudoku, int type, void (*pDPLL)(Formular&, Answer&));
void generateSudoku(int type, void (*pDPLL)(Formular&, Answer&));
#endif //SUDOKU_X_SUDOKU_H
