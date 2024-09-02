#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "SAT/include/SAT.h"
#include "OptSAT/include/OptSAT.h"
#include "X_Sudoku/include/X_Sudoku.h"

/*
 * 获取cnf的文件名
 */
char* getFileName(char* filepath){
    int len = strlen(filepath);
    char* ansName = (char *) malloc(sizeof(char)*len);
    char* p = strstr(filepath, ".cnf");
    for(;p >= filepath && *p != '/'; p --);
    p ++;

    int i;
    for(i = 0; *(p+i) != '.'; i ++){
        ansName[i] = *(p + i);
    }
    ansName[i] = '.';
    ansName[i+1] = 'r';
    ansName[i+2] = 'e';
    ansName[i+3] = 's';
    ansName[i+4] = '\0';
    char* tmp = (char*) malloc(sizeof(char) * (strlen("./Answer/") + len));
    strcpy(tmp, "./Answer/");
    strcat(tmp, ansName);
    free(ansName);
    ansName = tmp;
    return ansName;
}

bool fileExist(char* path){
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void writeAnswer(Answer ans, char* filepath){
    char* st = (char*) malloc(sizeof(char) * 15);
    strcpy(st, "./Answer");
    if(!fileExist(st))
        system("mkdir Answer");
    free(st);
    FILE* fout = fopen(filepath, "w+");
    if(fout == NULL){
        printf("Cannot open the file!\n");
        return;
    }
    if(!ans.solved)
        fprintf(fout, "0\n");
    else{
        for(int i = 1; i <= ans.numBoolen; i ++){
            if(ans.state[i] == POSITIVE)
                fprintf(fout, "%d ", i);
            else if(ans.state[i] == NEGATIVE)
                fprintf(fout, "-%d ", i);
            else
                fprintf(fout, "%d ", i);
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "time:%f\n", ans.time);
    fclose(fout);
}

void CNFtest(char* filepath, Answer (*DPLLSolution)(Formular&)){
    clock_t start, finish;
    Formular formular;
    formular.root = NULL;
    formular.numBoolen = 0;
    formular.numClause = 0;

    FILE* fin = fopen(filepath, "r");
    if(fin == NULL){
        printf("File not found!\n");
        return;
    }
    ReadCNFFile(fin, formular);
    fclose(fin);
    start = clock();
    Answer ans = DPLLSolution(formular);
    finish = clock();
    ans.time = (double)(finish - start) / CLOCKS_PER_SEC;

    if(ans.solved) {
        printf("SAT\n");
        for (int i = 1; i <= ans.numBoolen; i++) {
            if (ans.state[i] == POSITIVE)
                printf("%d ", i);
            else if(ans.state[i] == NEGATIVE)
                printf("-%d ", i);
            else
                printf("%d ", i);
        }
        printf("\n");
    }
    else
        printf("UNSOLVED!\n");
    char* ansName = getFileName(filepath);
    writeAnswer(ans, ansName);
    free(ansName);
    printf("Time: %f\n", ans.time);
    destroyFormular(formular);
    destroyAnswer(ans);
}

void testAnswer(){
    char filename[100];
    printf("Please input the cnf file path:");
    scanf("%s",filename);
    FILE* f_cnf = fopen(filename, "r");
    if(f_cnf == NULL){
        printf("cnf file not found!\n");
        return;
    }
    Formular formular;
    ReadCNFFile(f_cnf, formular);
    fclose(f_cnf);

    printf("Please input the answer file path:");
    scanf("%s", filename);
    FILE* f_ans = fopen(filename, "r");
    if(f_ans == NULL){
        printf("answer file not found!\n");
        return;
    }
    Answer ans;
    int boolen;
    fscanf(f_ans, "%d", &boolen);
    if(boolen == 0){
        DPLL(formular, ans);
        if(ans.solved)
            printf("Wrong Answer!\n");
        else
            printf("Right Answer!\n");
    }
    else{
        ans.state = (int*) malloc(sizeof(int) * (formular.numBoolen + 1));
        ans.state[1] = boolen > 0 ? POSITIVE : NEGATIVE;
        for(int i = 2; i <= formular.numBoolen; i ++){
            fscanf(f_ans, "%d", &boolen);
            ans.state[i] = boolen > 0 ? POSITIVE : NEGATIVE;
        }
        if(answerSatisfied(formular, ans))
            printf("Right Answer!\n");
        else
            printf("Wrong Answer!\n");
    }
}

void SAT_Solver_Menu(){
    printf("+++++++++++++++++++++++++++++++\n");
    printf("+         SAT-Solver          +\n");
    printf("+                             +\n");
    printf("+  1. Solve SAT problems      +\n");
    printf("+  2. Check Answer            +\n");
    printf("+  3. Exit                    +\n");
    printf("+                             +\n");
    printf("+++++++++++++++++++++++++++++++\n");
    int choice;
    printf("Please input your choice[1-3]:");
    scanf("%d", &choice);
    while(choice != 3){
        switch(choice){
            case 1:
                printf("1.Normal Algorithm\n");
                printf("2.Optimized Algorithm\n");
                printf("Please input the algorithm\nyou want to use[1-2]:\n");
                scanf("%d", &choice);
                Answer (*pDPLLSolution)(Formular&);
                if(choice == 1)
                    pDPLLSolution = DPLLSolution;
                else if(choice == 2)
                    ;
                else{
                    printf("Invalid input!\n");
                    break;
                }
                break;
            case 2:
                testAnswer();
                break;
            default:
                printf("Invalid input!\n");
                break;
        }
        printf("Please input your choice[1-3]:");
        scanf("%d", &choice);
    }
}

void Sudoku_Menu(){

}

int main() {
    test2();
    test3();
    printf("-----test-----\n");
    writeRules(NORMAL);
    writeRules(DIAGONAL);
    //printSudoku(generateSolutionTable(DIAGONAL));
    Sudoku s;
    initSudoku(s);
//    readSudokuTable(s, 1);
//    printTable(s.SolutionTable);
//    SolveSudokuTable(s.ProblemTable, DIAGONAL);
//    generateSolutionTable(DIAGONAL);
//    printTable(s.SolutionTable);
    generateSudoku(DIAGONAL, DPLL);
    SolveSudokuTable(s.ProblemTable, DIAGONAL, DPLL);
    printTable(s.ProblemTable);
    char* filepath = (char*)malloc(sizeof(char) * 100);
    for(int i = 0; i < 3; i ++){
        printf("Please input the path of the CNF file:\n");
        scanf("%s", filepath);
        CNFtest(filepath);
    }
    for(int i = 0; i < 3; i ++){
        testAnswer();
    }
    free(filepath);


    printf("------------------------------\n");
    printf("|                            |\n");
    printf("|          WELCOME           |\n");
    printf("|      author:Zhou Yujie     |\n");
    printf("|  Created in September,2024 |\n");
    printf("|                            |\n");
    printf("------------------------------\n");
    system("pause");
    system("cls");

    printf("------------------------------\n");
    printf("|           MENU              |\n");
    printf("|                             |\n");
    printf("|  1. SAT solver              |\n");
    printf("|  2. Sudoku                  |\n");
    printf("|  3. Exit                    |\n");
    printf("|                             |\n");
    printf("------------------------------\n");
    int choice;
    printf("Please input your choice[1-3]:");
    scanf("%d", &choice);
    while(choice != 3){
        switch(choice){
            case 1:
                SAT_Solver_Menu();
                break;
            case 2:
                Sudoku_Menu();
                break;
            default:
                printf("Invalid input!\n");
                break;
        }
        printf("Please input your choice[1-3]:");
        scanf("%d", &choice);
    }




    return 0;
}
