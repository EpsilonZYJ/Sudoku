/*
 * File: SAT.h
 * Created by Yujie Zhou
 * date: 2024-8-18
 */

#pragma once
#ifndef SUDOKU_SAT_H
#define SUDOKU_SAT_H

#define _CRT_SECURE_NO_WARNINGS

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
void addClause(Clause* &pre_clause, Clause* &insert_clause);
inline bool isUnitClause(Clause* clause);
void evaluateClause();

void test1();
void DPLL(Formular& formular);

#endif //SUDOKU_SAT_H
