#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "SAT/include/SAT.h"
#include "OptSAT/include/OptSAT.h"
#include "X_Sudoku/include/X_Sudoku.h"

#define abs(x) ((x) > 0 ? (x) : -(x))

/*
 * 获取要保存的res的文件路径
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

/*
 * 检测文件或者文件夹是否存在
 */
bool fileExist(char* path){
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

/*
 * 将答案存入指定路径
 */
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

/*
 * 测试DPLL算法
 */
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
                printf("%d\t", i);
            else if(ans.state[i] == NEGATIVE)
                printf("-%d\t", i);
            else
                printf("%d\t", i);
            if(i % 10 == 0)
                printf("\n");
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

/*
 * 测试答案是否正确
 */
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

/*
 * 获取cnf文件名
 */
char* getCNFFileName(char* filepath){
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
    ansName[i+1] = 'c';
    ansName[i+2] = 'n';
    ansName[i+3] = 'f';
    ansName[i+4] = '\0';
    return ansName;
}

/*
 * 输出优化率
 */
void printOptRate(char* filepath){
    FILE* fin = fopen(filepath, "r");
    if(fin == NULL){
        printf("File not found!\n");
        return;
    }
    char* filename = getCNFFileName(filepath);
    Formular formular;
    formular.root = NULL;
    formular.numBoolen = 0;
    formular.numClause = 0;
    clock_t start_origin, finish_origin, start_opt, finish_opt;

    ReadCNFFile(fin, formular);
    start_origin = clock();
    DPLLSolution(formular);
    finish_origin = clock();
    double time_origin = (double)(finish_origin - start_origin) / CLOCKS_PER_SEC;

    fseek(fin, 0, SEEK_SET);
    ReadCNFFile(fin, formular);
    start_opt = clock();
    OptDPLLSolution(formular);
    finish_opt = clock();
    double time_opt = (double)(finish_opt - start_opt) / CLOCKS_PER_SEC;
    fclose(fin);

    printf("%20s\t\t", filename);
    printf("%20.10f\t\t", time_origin);
    printf("%20.10f\t\t", time_opt);
    printf("%20.10f%%\n", (time_origin - time_opt)/time_origin*100);
    free(filename);
}

void OptRate(){
    char* filepath = (char*) malloc(sizeof(char) * 100);
    printf("\n\n\nOPTIMIZED RATE\n");
    printf("%20s\t\t%20s\t\t%20s\t\t%20s\n","FILE","Original algorithm","Optimized algorith","Optimized rate");

    strcpy(filepath, "./Data/sat-20.cnf");
    printOptRate(filepath);
    strcpy(filepath, "./Data/unsat-5cnf-30.cnf");
    printOptRate(filepath);
    strcpy(filepath, "./Data/ais10.cnf");
    printOptRate(filepath);
    strcpy(filepath, "./Data/sud00009.cnf");
    printOptRate(filepath);
    strcpy(filepath, "./Data/problem8-50.cnf");
    printOptRate(filepath);
    strcpy(filepath, "./Data/problem12-200.cnf");
    printOptRate(filepath);
    strcpy(filepath, "./Data/sud00001.cnf");
    printOptRate(filepath);
    strcpy(filepath, "./Data/sud00079.cnf");
    printOptRate(filepath);

    free(filepath);
}

/*
 * 求解指定数独
 */
void answerToGivenSudoku(Sudoku& sudoku, void (*pDPLL)(Formular&, Answer&), int line){
    initSudoku(sudoku);
    readSudokuTable(sudoku, line);
    printf("The problem table is:\n");
    printTable(sudoku.ProblemTable);
    writeRules(DIAGONAL);
    printf("Generating the answer...\r");
    time_t start, end;
    start = time(NULL);
    SolveSudokuTable(sudoku.ProblemTable, DIAGONAL, pDPLL);
    end = time(NULL);
    printf("Answer generated! (process time: %f s)\n", (double)(end-start));
    printTable(sudoku.ProblemTable);
}

void PlaySudoku(){
    Sudoku sudoku;
    int row, col, num;
    printf("Now you can play the diagonal Sudoku!\n");
    printf("The negative number means the number you filled in.\n");
    generateSudoku(sudoku, DIAGONAL, DPLL);
    printf("Please input the row[1-9], column[1-9] and\n number[1-9] you want to fill in.\n");
    printf("Input -1 -1 -1 to check the answer.\n");
    printf("Do you want to judge the answer when\n you are playing?(y/n)\n");
    char choice;
    while((choice = getchar()) != 'y' && choice != 'n');
    printf("Row:");
    scanf("%d", &row);
    printf("Column:");
    scanf("%d", &col);
    printf("Number:");
    scanf("%d", &num);
    while(row != -1 && col != -1 && num != -1){
        if(row < 1 || row > 9 || col < 1 || col > 9 || num < 1 || num > 9){
            printf("Invalid input!\n");
            system("pause");
            system("cls");
            printTable(sudoku.ProblemTable);
        }
        else if(sudoku.ProblemTable[row-1][col-1] > 0){
            printf("You cannot change the number!\n");
            system("pause");
            system("cls");
            printTable(sudoku.ProblemTable);
        }
        else{
            sudoku.ProblemTable[row-1][col-1] = -num;
            system("cls");
            printTable(sudoku.ProblemTable);
        }

        if(choice == 'y'){
            if(sudoku.SolutionTable[row-1][col-1] != num)
                printf("Wrong!\n");
            else
                printf("Right!\n");
        }
        printf("Input -1 -1 -1 to check the answer.\n");
        printf("Row:");
        scanf("%d", &row);
        printf("Column:");
        scanf("%d", &col);
        printf("Number:");
        scanf("%d", &num);
    }
    printf("Checking the answer...\n");
    for(int i = 0; i < 9; i ++){
        for(int j = 0; j < 9; j ++){
            if(abs(sudoku.ProblemTable[i][j]) != sudoku.SolutionTable[i][j]){
                printf("Wrong Answer!\n");
                printf("The correct answer is:\n");
                printTable(sudoku.SolutionTable);
                return;
            }
        }
    }
    printf("Right Answer!\n");
    return;
}

void showCNFInput(Formular formular, char* filepath){
    FILE* fin = fopen(filepath, "r");
    if(fin == NULL){
        printf("File not found!\n");
        return;
    }
    ReadCNFFile(fin, formular);
    fclose(fin);
    printf("The CNF formular is:\n");
    Clause* p = formular.root;
    while(p){
        Literal* q = p->firstLiteral;
        while(q){
            if(q->is_negative)
                printf("-");
            printf("%d\t", q->data);
            q = q->next;
        }
        printf("\n");
        p = p->nextClause;
    }
    printf("Boolen number: %d\n", formular.numBoolen);
    printf("Clause number: %d\n", formular.numClause);
}


/*
 * SAT-Solver菜单
 */
void SAT_Solver_Menu(){
    int choice = -1;
    char filepath[1000];
    system("cls");
    while (choice != 0){
        printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("+                    SAT-Solver                     +\n");
        printf("+                                                   +\n");
        printf("+             1. Solve SAT problems                 +\n");
        printf("+             2. Check Answer                       +\n");
        printf("+             3. Optimized Rate                     +\n");
        printf("+             4. Show CNF formular                  +\n");
        printf("+             0. Exit                               +\n");
        printf("+                                                   +\n");
        printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("Please input your choice[0-4]:");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                printf("\n\n1.Normal Algorithm\n");
                printf("2.Optimized Algorithm\n");
                printf("\nPlease input the algorithm you want to use[1-2]:");
                scanf("%d", &choice);
                Answer (*pDPLLSolution)(Formular&);
                if(choice == 1)
                    pDPLLSolution = DPLLSolution;
                else if(choice == 2)
                    pDPLLSolution = OptDPLLSolution;
                else{
                    printf("Invalid input!\n");
                    break;
                }
                printf("Please input the path of the CNF file:\n");
                scanf("%s", filepath);
                CNFtest(filepath, pDPLLSolution);
                break;
            case 2:
                testAnswer();
                break;
            case 3:
                OptRate();
                break;
            case 4:
                printf("Please input the path of the CNF file:\n");
                scanf("%s", filepath);
                Formular formular;
                formular.root = NULL;
                formular.numBoolen = 0;
                formular.numClause = 0;
                showCNFInput(formular, filepath);
                break;
            case 0:
                break;
            default:
                printf("Invalid input!\n");
                break;
        }
        if(choice != 0)
            system("pause");
        system("cls");
    }
}

/*
 * X-Sudoku菜单
 */
void Sudoku_Menu(){
    int choice = -1;
    system("cls");
    Sudoku sudoku;
    int line;
    void (*solver)(Formular&, Answer&);
    while (choice != 0){
        printf("*****************************************************\n");
        printf("*                     X-Sudoku                      *\n");
        printf("*                                                   *\n");
        printf("*      1. Generate a normal Sudoku                  *\n");
        printf("*      2. Generate a diagonal Sudoku                *\n");
        printf("*      3. The answer to the given Diagonal Sudoku   *\n");
        printf("*      4. Play the Diagonal Sudoku                  *\n");
        printf("*      0. Exit                                      *\n");
        printf("*                                                   *\n");
        printf("*****************************************************\n");
        printf("Please input your choice[0-3]:");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                printf("\n\n1.Normal Algorithm\n");
                printf("2.Optimized Algorithm\n");
                printf("\nPlease input the algorithm you want to use[1-2]:");
                scanf("%d", &choice);
                if(choice == 1)
                    solver = DPLL;
                else if(choice == 2)
                    solver = OptDPLL;
                else{
                    printf("Invalid input!\n");
                    break;
                }
                generateSudoku(sudoku, NORMAL, solver);
                printf("Press any key to get the answer...\n");
                system("pause");
                system("cls");
                printf("The answer to the normal Sudoku is:\n");
                printTable(sudoku.SolutionTable);
                break;
            case 2:
                printf("\n\n1.Normal Algorithm\n");
                printf("2.Optimized Algorithm\n");
                printf("\nPlease input the algorithm you want to use[1-2]:");
                scanf("%d", &choice);
                if(choice == 1)
                    solver = DPLL;
                else if(choice == 2)
                    solver = OptDPLL;
                else{
                    printf("Invalid input!\n");
                    break;
                }
                generateSudoku(sudoku, DIAGONAL, solver);
                printf("Press any key to get the answer...\n");
                system("pause");
                system("cls");
                printf("The answer to the diagonal Sudoku is:\n");
                printTable(sudoku.SolutionTable);
                break;
            case 3:
                printf("\n\n1.Normal Algorithm\n");
                printf("2.Optimized Algorithm\n");
                printf("\nPlease input the algorithm you want to use[1-2]:");
                scanf("%d", &choice);
                if(choice == 1)
                    solver = DPLL;
                else if(choice == 2)
                    solver = OptDPLL;
                else{
                    printf("Invalid input!\n");
                    break;
                }
                printf("Please input the line of the Sudoku table[1-20]:");
                scanf("%d", &line);
                answerToGivenSudoku(sudoku, solver, line);
                break;
            case 4:
                PlaySudoku();
                break;
            case 0:
                break;
            default:
                printf("Invalid input!\n");
                break;
        }
        if(choice != 0)
            system("pause");
        system("cls");
    }
}

int main() {
//    test2();
//    test3();
//    printf("-----test-----\n");
//    writeRules(NORMAL);
//    writeRules(DIAGONAL);
//    //printSudoku(generateSolutionTable(DIAGONAL));
//    Sudoku s;
//    initSudoku(s);
////    readSudokuTable(s, 1);
////    printTable(s.SolutionTable);
////    SolveSudokuTable(s.ProblemTable, DIAGONAL);
////    generateSolutionTable(DIAGONAL);
////    printTable(s.SolutionTable);
//    generateSudoku(DIAGONAL, DPLL);
//    SolveSudokuTable(s.ProblemTable, DIAGONAL, DPLL);
//    printTable(s.ProblemTable);
//    char* filepath = (char*)malloc(sizeof(char) * 100);
//    for(int i = 0; i < 3; i ++){
//        printf("Please input the path of the CNF file:\n");
//        scanf("%s", filepath);
//        CNFtest(filepath);
//    }
//    for(int i = 0; i < 3; i ++){
//        testAnswer();
//    }
//    free(filepath);

    //test();
    printf("----------------------------------------------------\n");
    printf("|                                                  |\n");
    printf("|                     WELCOME                      |\n");
    printf("|                 author:Zhou Yujie                |\n");
    printf("|             Created in September,2024            |\n");
    printf("|                                                  |\n");
    printf("----------------------------------------------------\n");
    printf("\n\n\n");
    printf("READ BEFORE USE:\n");
    printf("1. Do not change the relative path of Sudoku(.exe),\n Data folder and Answer folder.\n");
    printf("2. Do not delete the X-sudoku.txt in the Data folder.\n");
    printf("3. The CNF file could be put anywhere, but it would be\n better if you put it in the Data folder.\n");
    printf("4. All the answers to the CNF questions will be saved\n in the Answer folder.\n");
    printf("5. The given Sudoku file is in the Data folder,\n file name: X-sudoku.txt. You can choose the specific\n Sudoku by inputting the line of the Sudoku table.\n");
    printf("\nIf you are clear about these, press any key to continue...\n");
    system("pause");
    system("cls");

    int choice = -1;

    while(choice != 0){
        printf("-----------------------------------------------------\n");
        printf("|                      MENU                         |\n");
        printf("|                                                   |\n");
        printf("|             1. SAT solver                         |\n");
        printf("|             2. X-Sudoku                           |\n");
        printf("|             0. Exit                               |\n");
        printf("|                                                   |\n");
        printf("-----------------------------------------------------\n");
        printf("Please input your choice[0-2]:");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                SAT_Solver_Menu();
                break;
            case 2:
                Sudoku_Menu();
                break;
            case 0:
                break;
            default:
                printf("Invalid input!\n");
                break;
        }
    }


    return 0;
}
