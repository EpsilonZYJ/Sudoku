#include "include/SAT.h"

/*
 * 读取cnf文件并且存储为相应的数据结构
 */
void ReadCNFFile(FILE* fopen){
    char *buf = (char *) malloc(sizeof(char) * 200);

    //处理注释说明行
    fgets(buf, 199, fopen);
    while(*buf != 'p')
        fgets(buf, 199, fopen);

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
        while(fscanf(fopen, "%d", &x) && x != 0){

        }
    }
}

void createClause(){

}

void destroyClause(){

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
