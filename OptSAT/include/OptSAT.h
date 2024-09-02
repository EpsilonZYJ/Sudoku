#pragma once
#ifndef SUDOKU_OPTSAT_H
#define SUDOKU_OPTSAT_H

#include <stdio.h>
#include "../../SAT/include/SAT.h"

#define UNKNOWN 0

#define CONFLICT 1
#define UNSATISFIABLE 2
#define SATISFIABLE 3

typedef struct OptLiteral{
    int data;
    struct Literal* next;
}OptLiteral;

typedef struct OptClause{
    struct OptClause* nextClause;
    Literal* firstLiteral;
}OptClause;

typedef struct OptFormular{
    OptClause* root;
    int numClause;
    int numBoolen;
}OptFormular;

typedef struct LiteralWatch{
    OptLiteral* positive;
    OptLiteral* negative;
};

typedef struct OptAnswer{

};

int decide_next_branch();
void deduce();
int analyze_conflict();


void test3();

#endif //SUDOKU_OPTSAT_H
