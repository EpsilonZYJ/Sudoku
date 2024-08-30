#include <stdio.h>
#include "include/X_Sudoku.h"
#include "../SAT/include/SAT.h"

Hole randomLocation(){
    Hole h;
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
            int num = sudoku.ProblemTable[row-1][col-1];
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

void test2(){
    printf("---X_Sudo---\n");
}