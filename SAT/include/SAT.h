/*
 * File: SAT.h
 * Created by Yujie Zhou
 * date: 2024-8-18
 */

#pragma once
#ifndef SUDOKU_SAT_H
#define SUDOKU_SAT_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//文字的存储结构
typedef struct Literal{
    int data;
    bool is_negative;
    struct Literal* next;
}Literal, *pLiteral;


typedef struct Clause{
    struct Clause* nextClause;
    Literal* firstLiteral;
}Clause, *pClause;

typedef struct Formular{
    pClause root;
    int numClause;
    int numBoolen;
}Formular;

void ReadCNFFile(FILE* fin);

Clause* createClause(Literal* &head);
void destroyClause(Formular& formular, Clause* &clause);
void addClause();
void removeClause();
void isUnitClause();
void evaluateClause();

void test1();

#endif //SUDOKU_SAT_H
