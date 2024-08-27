#include <stdio.h>
#include "SAT/include/SAT.h"
#include "OptSAT/include/OptSAT.h"
#include "X_Sudoku/include/X_Sudoku.h"

void CNFtest(char* filepath){
    clock_t start, finish;
    start = clock();
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
    Answer ans = DPLLSolution(formular);
    finish = clock();
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;

    if(ans.solved) {
        printf("SAT\n");
        for (int i = 1; i <= ans.numBoolen; i++) {
            if (ans.state[i] == POSITIVE)
                printf("%d ", i);
            else if(ans.state[i] == NEGATIVE)
                printf("-%d ", i);
            else
                printf("+-%d ", i);
        }
        printf("\n");
    }
    else
        printf("UNSOLVED!\n");
    printf("Time: %f\n", duration);
}

void test_destroyFormular(){
    Formular formulatr;
    formulatr.root = NULL;
    formulatr.numBoolen = 0;
    formulatr.numClause = 0;
    printf("---read file---\n");
    char* filepath = (char*)malloc(sizeof(char) * 100);
    scanf("%s", filepath);
    FILE* fp = fopen(filepath, "r");
    if(fp == NULL){
        printf("File not found!\n");
        return;
    }
    ReadCNFFile(fp, formulatr);
    fclose(fp);
    printf("%p\n", formulatr.root);
    printf("---destroy---\n");
    destroyFormular(formulatr);


    fp = fopen(filepath, "r");
    ReadCNFFile(fp, formulatr);
    printf("%p\n", formulatr.root);
    fclose(fp);
    printf("---destroy---\n");
    Formular newFormular = copyFormular(formulatr);
    destroyFormular(formulatr);

    printf("newFormular\n");
    printf("%p\n", newFormular.root);
    destroyFormular(newFormular);

}

int main() {
    test1();
    test2();
    test3();
    printf("-----test-----\n");
    char* filepath = (char*)malloc(sizeof(char) * 100);
    while(1){
        printf("Please input the path of the CNF file:\n");
        scanf("%s", filepath);
        CNFtest(filepath);
    }
//    test_destroyFormular();
    return 0;
}
