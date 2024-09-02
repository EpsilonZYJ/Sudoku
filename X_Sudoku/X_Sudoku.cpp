#include <stdio.h>
#include "include/X_Sudoku.h"
#include "../SAT/include/SAT.h"

Hole randomLocation(){
    Hole h;
    srand(time(0));
    h.x = rand() % 9;
    h.y = rand() % 9;
    return h;
}

bool rulesExist(char* filepath){
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}

inline int getLiteral(int row, int col, int num){
    return (row-1)*81 + (col-1)*9 + num;
}

PosNum decodeLiteral(int boolen){
    PosNum pos;
    pos.num = (boolen-1) % 9 + 1;
    boolen = (boolen-1)/9;
    pos.y = boolen % 9 + 1;
    boolen = boolen / 9;
    pos.x = boolen + 1;
    return pos;
}

void writeRules(int type){
     char* s;
    if(type == NORMAL){
        s = (char*) malloc(sizeof(char)*20);
        strcpy(s, "SudokuRules.cnf");
    }
    else{
        s = (char*) malloc(sizeof(char)*30);
        strcpy(s, "DiagonalSudokuRules.cnf");
    }
    if(rulesExist(s))
        return;
    FILE* fout = fopen(s, "w+");
    int row, col, num;

    if(type == DIAGONAL){
        fprintf(fout, "p cnf 729 10935\n");
    }
    else{
        fprintf(fout, "p cnf 729 10287\n");
    }

    //每个格子只能填入1~9中的一个数字
    for(row = 1; row <=9; row ++){
        for(col = 1; col <= 9; col ++){
            for(num = 1; num <= 8; num ++){
                for(int i = num + 1; i <= 9; i ++){
                    fprintf(fout, "-%d -%d 0\n", getLiteral(row, col, num), getLiteral(row, col, i));
                }
            }
        }
    }

    //每行每个数字至多出现一次
    for(row = 1; row <= 9; row ++){
        for(num = 1; num <= 9; num ++){
            for(col = 1; col <= 8; col ++){
                for(int i = col+1; i <= 9; i ++){
                    fprintf(fout, "-%d -%d 0\n", getLiteral(row, col, num), getLiteral(row, i, num));
                }
            }
        }
    }

    //每列每个数字至多出现一次
    for(col = 1; col <= 9; col ++){
        for(num = 1; num <= 9; num ++){
            for(row = 1; row <= 8; row ++){
                for(int i = row+1; i <= 9; i ++){
                    fprintf(fout, "-%d -%d 0\n", getLiteral(row, col, num), getLiteral(i, col, num));
                }
            }
        }
    }

    //每个3*3的小方格中每个数字至多出现一次
    for(int row_i = 0; row_i < 3; row_i ++){
        for(int col_i = 0; col_i < 3; col_i ++){
           //第row_i行col_i列的3x3方格
           for(num = 1; num <= 9; num ++){
               for(row = 1; row <=3; row ++){
                   for(col = 1; col <= 3; col ++){
                       for(int i = row+1; i <= 3; i ++){
                           for(int j = 1; j <= 3; j ++){
                               if(col == j)
                                   continue;
                               fprintf(fout, "-%d -%d 0\n", getLiteral(row_i*3+row, col_i*3+col, num), getLiteral(row_i*3+i, col_i*3+j, num));
                           }
                       }
                   }
               }
           }
        }
    }

    //每个3x3的小方格中每个数字至少出现一次
    for(num = 1; num <= 9; num ++){
        for(int row_i = 0; row_i <= 2; row_i ++){
            for(int col_i = 0; col_i <= 2; col_i ++){
                for(row = 1; row <= 3; row ++){
                    for(col = 1; col <= 3; col ++){
                        fprintf(fout, "%d ", getLiteral(row_i*3+row, col_i*3+col, num));
                    }
                }
                fprintf(fout, "0\n");
            }
        }
    }

    if(type == DIAGONAL){
        //对角线上的数字不能重复
        for(row = 1; row <= 9; row ++){
            for(num = 1; num <= 9; num ++){
                for(int i = row+1; i <= 9; i ++){
                    fprintf(fout, "-%d -%d 0\n", getLiteral(row, row, num), getLiteral(i, i, num));
                }
            }
        }

        for(row = 1; row <= 9; row ++){
            for(num = 1; num <= 9; num ++){
                for(int i = row+1; i <= 9; i ++){
                    fprintf(fout, "-%d -%d 0\n", getLiteral(row, 10-row, num), getLiteral(i, 10-i, num));
                }
            }
        }
    }
    fclose(fout);
}

Answer encodeSudokuTable(Sudoku sudoku){
    Answer ans;
    ans.state = (int*) malloc(sizeof(int)* 730);
    for(int i = 1; i < 730; i ++)
        ans.state[i] = UNKNOWN;
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            int num = sudoku.SolutionTable[row-1][col-1];
            if(num == EMPTY)
                continue;
            ans.state[getLiteral(row, col, num)] = POSITIVE;
            for(int i = 1; i <= 9; i ++){
                if(i == num)
                    continue;
                ans.state[getLiteral(row, col, i)] = NEGATIVE;
            }
        }
    }
    return ans;
}

Answer encodeTable(int table[9][9]){
    Answer ans;
    ans.state = (int*) malloc(sizeof(int)* 730);
    for(int i = 1; i < 730; i ++)
        ans.state[i] = UNKNOWN;
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            int num = table[row-1][col-1];
            if(num == EMPTY)
                continue;
            ans.state[getLiteral(row, col, num)] = POSITIVE;
            for(int i = 1; i <= 9; i ++){
                if(i == num)
                    continue;
                ans.state[getLiteral(row, col, i)] = NEGATIVE;
            }
        }
    }
    return ans;
}

void initSudoku(Sudoku& s){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            s.ProblemTable[i][j] = EMPTY;
            s.SolutionTable[i][j] = EMPTY;
        }
    }
    s.numFilled = 0;
    s.numHoles = 0;
}

bool checkOneSudokuAnswer(Answer ans){
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            for(int num = 1; num <= 9; num ++){
                if(ans.state[getLiteral(row, col, num)] == UNKNOWN)
                    return false;
            }
        }
    }
    return true;
}

void tableFormularAdd(Formular& formular, int table[9][9]){
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            if(table[row-1][col-1] != EMPTY){
                int num = table[row-1][col-1];
                Literal *head = (Literal*) malloc(sizeof(Literal));
                head->data = getLiteral(row, col, num);
                head->is_negative = false;
                head->next = NULL;
                Clause* clause = createClause(head);
                addClause(formular, formular.root, clause);
            }
        }
    }
}

void simplifyFormular(Formular& formular, Answer ans){
    Clause* s = formular.root;
    while(s){
        if(s->firstLiteral->is_negative == false && ans.state[s->firstLiteral->data] == POSITIVE){
            Clause* p = s;
            s = s->nextClause;
            destroyClause(formular, p);
        }
        else if(s->firstLiteral->is_negative == true && ans.state[s->firstLiteral->data] == NEGATIVE){
            Clause* p = s;
            s = s->nextClause;
            destroyClause(formular, p);
        }
        else if(s->firstLiteral->is_negative == false && ans.state[s->firstLiteral->data] == NEGATIVE){
            pLiteral q = s->firstLiteral;
            s->firstLiteral = s->firstLiteral->next;
            free(q);
            s = s->nextClause;
        }
        else if(s->firstLiteral->is_negative == true && ans.state[s->firstLiteral->data] == POSITIVE){
            pLiteral q = s->firstLiteral;
            s->firstLiteral = s->firstLiteral->next;
            free(q);
            s = s->nextClause;
        }
        else{
            pLiteral pre = s->firstLiteral;
            bool next = false;
            while(pre && pre->next){
                if(pre->next->is_negative == false && ans.state[pre->next->data] == POSITIVE){
                    Clause* p = s;
                    s = s->nextClause;
                    destroyClause(formular, p);
                    next = true;
                    while(s->firstLiteral == NULL)
                        s = s->nextClause;
                    pre = s->firstLiteral;
                }
                else if(pre->next->is_negative == true && ans.state[pre->next->data] == NEGATIVE){
                    Clause* p = s;
                    s = s->nextClause;
                    destroyClause(formular, p);
                    next = true;
                    while(s->firstLiteral == NULL)
                        s = s->nextClause;
                    pre = s->firstLiteral;
                }
                else if(pre->next->is_negative == false && ans.state[pre->next->data] == NEGATIVE){
                    pLiteral q = pre->next;
                    pre->next = pre->next->next;
                    free(q);
                    pre = pre->next;
                }
                else if(pre->next->is_negative == true && ans.state[pre->next->data] == POSITIVE){
                    pLiteral q = pre->next;
                    pre->next = pre->next->next;
                    free(q);
                    pre = pre->next;
                }
                else
                    pre = pre->next;
            }
            if(!next)
                s = s->nextClause;
        }
        if(formular.root == NULL){
            ans.solved = true;
            return;
        }
    }
    for(int row = 1; row <= 9; row  ++){
        for(int col = 1; col <= 9; col ++){
            for(int num = 1; num <= 9; num ++){
                if(ans.state[getLiteral(row, col, num)] != EMPTY)
                    formular.numBoolen --;
            }
        }
    }
}

void test2(){
    printf("---X_Sudo---\n");
}

void printSudoku(Sudoku sudoku){
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            if(sudoku.SolutionTable[row-1][col-1] == EMPTY)
                printf("* ");
            else
                printf("%d ", sudoku.SolutionTable[row-1][col-1]);
            if(col % 3 == 0)
                printf("|");
        }
        printf("\n");
        if(row % 3 == 0 && row != 9)
            printf("----------------------\n");
    }
}

void printTable(int table[9][9]){
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            if(table[row-1][col-1] == EMPTY)
                printf("* ");
            else
                printf("%d ", table[row-1][col-1]);
            if(col % 3 == 0)
                printf("|");
        }
        printf("\n");
        if(row % 3 == 0 && row != 9)
            printf("----------------------\n");
    }
}

bool SolveSudokuTable(int table[9][9], int type){
    writeRules(type);
    FILE* fin;
    if(type == DIAGONAL){
        fin = fopen("DiagonalSudokuRules.cnf", "r");
    }
    else{
        fin = fopen("SudokuRules.cnf", "r");
    }
    Formular formular;
    Answer ans;
    ReadCNFFile(fin, formular);
    ans = encodeTable(table);

    tableFormularAdd(formular, table);
    DPLL(formular, ans);
    if(ans.solved == false){
        destroyFormular(formular);
        destroyAnswer(ans);
        fclose(fin);
        return false;
    }
    for(int rowi = 1; rowi <= 9; rowi ++){
        for(int coli = 1; coli <= 9; coli ++){
            for(int numi = 1; numi <= 9; numi ++){
                if(ans.state[getLiteral(rowi, coli, numi)] == POSITIVE){
                    table[rowi-1][coli-1] = numi;
                }
            }
        }
    }
    destroyFormular(formular);
    destroyAnswer(ans);
    fclose(fin);
    return true;
}

bool las_vegas(int n, Sudoku& sudoku, int type){
    Position pos;
    for(int i = 0; i < n; i ++){
        Position pos = randomLocation();
        while(sudoku.SolutionTable[pos.x][pos.y] != EMPTY)
            pos = randomLocation();
        int num = i % 9 + 1;
        sudoku.SolutionTable[pos.x][pos.y] = num;
        sudoku.numFilled ++;
    }
    if (SolveSudokuTable(sudoku.SolutionTable, type) == true)
        return true;
    return false;
}

Sudoku generateSolutionTable(int type){
    Sudoku sudoku;
    initSudoku(sudoku);
    while (!las_vegas(11, sudoku, type)) {
        printSudoku(sudoku);
        initSudoku(sudoku);
        printf("@@@@@@@@@@@@@@@@@@@@\n");
    }
    return sudoku;
}



Sudoku sudokuSolution(void (*DPLLSolu)(Formular&, Answer& ans), int type){
    Sudoku sudoku;
    initSudoku(sudoku);
    Formular formular;
    Answer ans;
    ans.solved = false;

    //生成数独规则
    writeRules(type);
    FILE* fin;
    if(type == DIAGONAL){
        fin = fopen("DiagonalSudokuRules.cnf", "r");
    }
    else{
        fin = fopen("SudokuRules.cnf", "r");
    }

    printf("We are now generating the solution of sudoku...\n");

    do{
        int percent = sudoku.numFilled * 100 / 81;
        printf("%d%%", percent);
        for(int i = 0; i < percent/10; i ++)
            printf("#");
        printf("\r");
        fflush(stdout);

        ReadCNFFile(fin, formular);
        if(81 - sudoku.numFilled > 11){
            for(int i = 0; i < 11; i ++){
                Position pos = randomLocation();
                while(sudoku.SolutionTable[pos.x][pos.y] != EMPTY)
                    pos = randomLocation();
                int num = rand() % 9 + 1;
                sudoku.SolutionTable[pos.x][pos.y] = num;
                sudoku.numFilled ++;
            }
        }
        percent = sudoku.numFilled * 100 / 81;
        printf("%d%%", percent);
        for(int i = 0; i < percent/10; i ++)
            printf("#");
        printf("\r");
        fflush(stdout);

        ans = encodeSudokuTable(sudoku);
        simplifyFormular(formular, ans);
        DPLLSolu(formular, ans);
        if(ans.solved == true){
            for(int row = 1; row <= 9; row ++){
                for(int col = 1; col <= 9; col ++){
                    for(int num = 1; num <= 9; num ++){
                        if(ans.state[getLiteral(col, row, num)] == POSITIVE && sudoku.SolutionTable[row-1][col-1] == EMPTY){
                            sudoku.SolutionTable[row-1][col-1] = num, sudoku.numFilled ++;
                        }
                    }
                }
            }
            if(sudoku.numFilled == 81){
                break;
            }
        }
        else{
            initSudoku(sudoku);
        }
        destroyAnswer(ans);
    }while(1);

    destroyFormular(formular);
    destroyAnswer(ans);
    fclose(fin);

    return sudoku;
}

void readSudokuTable(Sudoku& sudoku, int LineNum){
    FILE* fin = fopen("../Data/X-sudoku.txt", "r");
    if(fin == NULL){
        printf("File not found!\n");
        return;
    }
    char line[90];
    for(int i = 0; i < LineNum; i ++)
        fgets(line, 85, fin);
    for(int row = 0; row < 9; row ++){
        for(int col = 0; col < 9; col ++){
            char c = line[row*9+col];
            if(c == '0')
                sudoku.ProblemTable[row][col] = EMPTY;
            else
                sudoku.ProblemTable[row][col] = c - '0';
        }
    }
    fclose(fin);
}
