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
            if (ans.state[i])
                printf("%d ", i);
            else
                printf("-%d ", i);
        }
        printf("\n");
    }
    else
        printf("UNSOLVED!\n");
    printf("Time: %f\n", duration);
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
    return 0;
}
