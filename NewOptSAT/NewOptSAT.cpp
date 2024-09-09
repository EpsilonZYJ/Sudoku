#include "include/NewOptSAT.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))

int branchDepth;

void test(){
    printf("Hello World!\n");
}

void NewReadCNFFile(FILE* fin, Formular& formular){
    char *buf = (char *) malloc(sizeof(char) * 200);

    //处理注释说明行
    fgets(buf, 199, fin);
    while(*buf != 'p')
        fgets(buf, 199, fin);

    //统计布尔变元个数和子句个数
    unsigned int literals = 0;
    unsigned int lines = 0;
    char *p = buf;
    while(*p > '9' || *p < '0') p ++;
    while(*p != ' '){
        literals = literals*10 + (*p - '0');
        p ++;
    }
    while(*p == ' ')    p ++;
    while(*p >= '0' && *p <= '9'){
        lines = lines*10 + (*p - '0');
        p ++;
    }
    p = NULL;

    formular.numBoolen = (int)literals;
    formular.numClause = (int)lines;
    //读取布尔变元并进行存储
    pLiteral phead = (pLiteral) malloc(sizeof(Literal));
    litsNums = (LiteralNums*) malloc(sizeof(LiteralNums) * (literals + 1));
    phead->next = NULL;
    for(unsigned int i = 0; i < lines; i ++){
        int x;
        int count = 0;
        while(fscanf(fin, "%d", &x) && x != 0){
            pLiteral pL = (pLiteral) malloc(sizeof(Literal));
            pL->is_negative = x < 0;
            pL->data = ABS(x);
            pL->next = phead->next;
            phead->next = pL;
            count ++;
            litsNums[ABS(x)].Num ++;
            if(x > 0)
                litsNums[ABS(x)].PositiveNum ++;
            else
                litsNums[ABS(x)].NegativeNum ++;
        }
        Clause* clause = createClause(phead->next);
        clause->numLiteral = count;
        phead->next = NULL;
        if(i == 0){
            formular.root = clause;
            clause->nextClause = NULL;
        }else{
            Clause* pC = formular.root;
            clause->nextClause = pC;
            formular.root = clause;
        }
    }
}

int decide_next_branch(Answer ans, LiteralNums DecideLiteral[]){
    int maxNum = 0;
    int maxLiteral = 0;
    if(branchDepth == 1){
        for(int i = 1; i <= ans.numBoolen; i ++){
            if(ans.state[i] == UNKNOWN){
                if(litsNums[i].PositiveNum > maxNum){
                    maxNum = litsNums[i].PositiveNum;
                    maxLiteral = i;
                }
                else if(litsNums[i].NegativeNum > maxNum){
                    maxNum = litsNums[i].NegativeNum;
                    maxLiteral = -i;
                }
            }
        }
    }
    else{
        for(int i = 1; i <= ans.numBoolen; i ++){
            if(ans.state[i] == UNKNOWN){
                if(DecideLiteral[i].PositiveNum > maxNum){
                    maxNum = DecideLiteral[i].PositiveNum;
                    maxLiteral = i;
                }
                else if(litsNums[i].NegativeNum > maxNum){
                    maxNum = DecideLiteral[i].NegativeNum;
                    maxLiteral = -i;
                }
            }
        }
    }
    return maxLiteral;
}

int deduce(){

    return 0;
}

int analyze_conflict(){
    return 0;
}

void backtrack(int blevel){

}

int NewOptDPLL(Answer ans, LiteralNums DecideLiteral[]){
    int status, blevel;
    while(true){
        int branch = decide_next_branch(ans, DecideLiteral);
        while(true){
            status = deduce();
            if(status == CONFLICT){
                blevel = analyze_conflict();
                if(blevel == 0){
                    return UNSATISAIBLE;
                }
                else{
                    backtrack(blevel);
                }
            }
            else if(status == SATISFIABLE){
                return SATISFIABLE;
            }
            else break;
        }
    }
}