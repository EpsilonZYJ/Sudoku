#pragma once
#ifndef SUDOKU_SAT_H
#define SUDOKU_SAT_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct Literal{
    int data;
    bool is_negative;
    struct Literal* next;
}Literal, *pLiteral;

typedef struct Clause{

}Clause;


void ReadCNFFile(FILE* fopen);

void createClause();
void destroyClause();
void addClause();
void removeClause();
void isUnitClause();
void evaluateClause();

void test1();

#endif //SUDOKU_SAT_H
