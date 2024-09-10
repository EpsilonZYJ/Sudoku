#pragma once
#ifndef SUDOKU_NEWOPTSAT_H
#define SUDOKU_NEWOPTSAT_H
#include <stdio.h>
#include "../../SAT/include/SAT.h"
#include "../../OptSAT/include/OptSAT.h"

#define CONFLICT -1
#define UNSATISAIBLE 0
#define SATISFIABLE 1

#define LITERAL_MAX_NUMS 10000

typedef struct LiteralClause{
    Clause* clause;
    struct LiteralClause* next;
}LiteralClause;

typedef struct BiWatchingList{
    LiteralClause* positive;
    LiteralClause* negative;
}BiWatchingList;

void test();
int decide_next_branch(Answer ans, LiteralNums DecideBranch[]);
int deduce();
int analyze_conflict();
void backtrack(int blevel);
int NewOptDPLL(Answer& ans, LiteralNums DecideBranch[]);
void NewDPLLSolution();



#endif //SUDOKU_NEWOPTSAT_H
