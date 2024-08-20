#include "include/SAT.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))

/*
 * 读取cnf文件并且存储为相应的数据结构
 */
void ReadCNFFile(FILE* fin, Formular& formular){
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
    phead->next = NULL;
    for(unsigned int i = 0; i < lines; i ++){
        int x;
        while(fscanf(fin, "%d", &x) && x != 0){
            pLiteral pL = (pLiteral) malloc(sizeof(Literal));
            pL->is_negative = x < 0;
            pL->data = ABS(x);
            pL->next = phead->next;
            phead->next = pL;
        }
        Clause* clause = createClause(phead->next);
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

/*
 * 创建子句
 */
Clause* createClause(Literal* &head){
    Clause* clause = (Clause *) malloc(sizeof(Clause));
    clause->firstLiteral = head;
    return clause;
}

/*
 * 销毁子句
 */
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

/*
 * 向pre_clause子句后插入insert_clause子句
 */
void addClause(Clause* &pre_clause, Clause* &insert_clause){
    Clause *p = pre_clause->nextClause;
    pre_clause->nextClause = insert_clause;
    insert_clause->nextClause = p;
}

/*
 * 判断子句是否为单子句
 */
inline bool isUnitClause(Clause* clause){
    return clause->firstLiteral->next == NULL;
}

void evaluateClause(){

}

void DPLL(Formular& formular){

}

void test1(){
    printf("----SAT----\n");
}
