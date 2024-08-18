#include "include/SAT.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))

/*
 * 读取cnf文件并且存储为相应的数据结构
 */
void ReadCNFFile(FILE* fin){
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

    //读取布尔变元并进行存储
    for(unsigned int i = 0; i < lines; i ++){
        int x;
        while(fscanf(fin, "%d", &x) && x != 0){

        }
    }
}

Clause* createClause(Literal* &head){
    Clause* clause = (Clause *) malloc(sizeof(Clause));
    clause->firstLiteral = head;
    return clause;
}

void destroyClause(Formular& formular, Clause* &clause){
    pLiteral p = clause->firstLiteral;
    while(clause->firstLiteral){
        clause->firstLiteral = p->next;
        free(p);
    }
    Clause *cls_free = formular.root;
    if(formular.root == clause){
        formular.root = cls_free->nextClause;
        free(cls_free);
        cls_free = NULL;
    }else{
        while(cls_free->nextClause && cls_free->nextClause != clause)
            cls_free = cls_free->nextClause;
        Clause *s = cls_free->nextClause;
        cls_free->nextClause = s->nextClause;
        free(s);
        s = NULL;
    }
    formular.numClause --;
}

void addClause(){

}

void removeClause(){

}

void isUnitClause(){

}

void evaluateClause(){

}

void test1(){
    printf("----SAT----\n");
}
