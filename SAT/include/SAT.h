/*
 * SAT.h
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

#define POSITIVE 1
#define NEGATIVE -1
#define UNKNOWN 0

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

typedef struct Answer{
    int numBoolen;
    bool solved;
    int* state;
}Answer;

void ReadCNFFile(FILE* fin, Formular& formular);

Clause* createClause(Literal* &head);
void destroyClause(Formular& formular, Clause* &clause);
void addClause(Formular &formular, Clause* &pre_clause, Clause* &insert_clause);
inline bool isUnitClause(Clause* clause);
void evaluateClause();
Clause* UnitClauseLeft(Formular formular);

void test1();
void DPLL(Formular &formular, Answer& ans);
Answer DPLLSolution(Formular& formular);
void destroyFormular(Formular& formular);
void destroyAnswer(Answer& ans);
Formular copyFormular(Formular formular);

#endif //SUDOKU_SAT_H
