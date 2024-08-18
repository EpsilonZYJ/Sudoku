#pragma once
#ifndef SUDOKU_SAT_H
#define SUDOKU_SAT_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct literal{
    int data;
    bool is_negative;
    struct literal* next;
}Literal, *pLiteral;

void ReadCNFFile(FILE* fopen);

void createClause();
void destroyClause();
void addClause();
void removeClause();
void isUnitClause();
void evaluateClause();

void test1();

#endif //SUDOKU_SAT_H
