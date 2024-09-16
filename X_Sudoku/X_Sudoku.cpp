/*
 * X_Sudoku.cpp
 * Created by Yujie Zhou
 * date: 2024-9-2
 */
#include <stdio.h>
#include <time.h>
#include "include/X_Sudoku.h"
#include "../SAT/include/SAT.h"

/*
 * 生成随机位置
 */
Hole randomLocation(){
    Hole h;
    srand(time(0));
    h.x = rand() % 9;
    h.y = rand() % 9;
    return h;
}

/*
 * 判断规则文件是否存在
 */
bool rulesExist(char* filepath){
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}

/*
 * 根据行列和数字生成/编码为对应文字
 */
inline int getLiteral(int row, int col, int num){
    return (row-1)*81 + (col-1)*9 + num;
}

/*
 * 根据数独类型生成对应规则文件
 */
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

/*
 * 编码已有的数独表格
 */
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

/*
 * 初始化数独表格
 */
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

/*
 * 检查是否有唯一解
 */
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

/*
 * 将数独盘面已有数字加入到约束规则中
 */
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

/*
 * 打印数独
 */
void printTable(int table[9][9]){
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            if(table[row-1][col-1] == EMPTY)
                printf("* ");
            else
                printf("%-2d", table[row-1][col-1]);
            if(col % 3 == 0)
                printf("|");
        }
        printf("\n");
        if(row % 3 == 0 && row != 9)
            printf("----------------------\n");
    }
}

/*
 * 解数独
 */
bool SolveSudokuTable(int table[9][9], int type, void (*pDPLL)(Formular&, Answer&)){
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
    pDPLL(formular, ans);
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

/*
 * 从指定文件指定行中读取数独
 */
void readSudokuTable(Sudoku& sudoku, int LineNum){
    FILE* fin = fopen("./Data/X-sudoku.txt", "r");
    if(fin == NULL){
        printf("File (Path: ./Data/X-sudoku.txt) not found!\n");
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

/*
 * 交换两个整数
 */
void swap(int& a, int& b){
    int temp = a;
    a = b;
    b = temp;
}

/*
 * 挖洞法生成数独
 */
void DigHole(Sudoku& sudoku, int type, void (*pDPLL)(Formular&, Answer&)){
    FILE* fin;
    if(type == DIAGONAL){
        fin = fopen("DiagonalSudokuRules.cnf", "r");
    }
    else{
        fin = fopen("SudokuRules.cnf", "r");
    }
    printf("Digging holes...\n");
    printf("%d%%\r", 0);
    int order[81];
    for(int i = 0; i < 81; i ++)
        order[i] = i;
    for(int i = 80; i > 0; i --){
        int j = rand() % 81;
        if(i != j)
            swap(order[i], order[j]);
    }
    int h = 0;
    while(true){
        sudoku.ProblemTable[order[h]/9][order[h]%9] = EMPTY;
        sudoku.numHoles ++;
        Formular formular;
        fseek(fin, 0, SEEK_SET);
        ReadCNFFile(fin, formular);
        Answer ans;
        ans = encodeTable(sudoku.ProblemTable);
        tableFormularAdd(formular, sudoku.ProblemTable);
        pDPLL(formular, ans);
        if(checkOneSudokuAnswer(ans) == false){
            sudoku.ProblemTable[order[h]/9][order[h]%9] = sudoku.SolutionTable[order[h]/9][order[h]%9];
            destroyFormular(formular);
            destroyAnswer(ans);
            break;
        }
        else if(sudoku.numHoles == MAX_HOLES){
            break;
        }

        printf("%2d%%|", (int)(sudoku.numHoles*100/64.0));
        for(int i = 0; i < (int)(sudoku.numHoles*10/64); i ++)
            printf("#");
        for(int i = 0; i < 10 - (int)(sudoku.numHoles*10/64); i ++)
            printf(" ");
        printf("|");
        printf("\r");
        destroyFormular(formular);
        destroyAnswer(ans);
        h++;
    }
    printf("Generate the Sudoku problem successfully!\n");
    fclose(fin);
}

/*
 * 生成数独
 */
void generateSudoku(Sudoku& sudoku, int type, void (*pDPLL)(Formular&, Answer&)){
    initSudoku(sudoku);
    if(type == DIAGONAL){
        writeRules(DIAGONAL);
    }
    else{
        writeRules(NORMAL);
    }
    int randNum = rand() % 20 + 1;
    readSudokuTable(sudoku, randNum);

    time_t start, end;
    start = time(NULL);
    printf("Generating the answer...\r");
    SolveSudokuTable(sudoku.ProblemTable, type, pDPLL);
    end = time(NULL);
    printf("Answer generated! (process time: %Lf s)\n", (long double)(end-start));

    //获得答案
    for(int row = 1; row <= 9; row ++){
        for(int col = 1; col <= 9; col ++){
            sudoku.SolutionTable[row-1][col-1] = sudoku.ProblemTable[row-1][col-1];
        }
    }

    //挖洞法生成数独
    DigHole(sudoku, type, pDPLL);
    printTable(sudoku.ProblemTable);
}