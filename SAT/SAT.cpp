/*
 * SAT.cpp
 * Created by Yujie Zhou
 * date: 2024-8-18
 */
#include "include/SAT.h"
#include <assert.h>

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
    Literal* p = clause->firstLiteral;
    while(clause->firstLiteral){
        clause->firstLiteral = p->next;
        free(p);
        p = clause->firstLiteral;
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
void addClause(Formular& formular, Clause* &pre_clause, Clause* &insert_clause){
    Clause *p = pre_clause->nextClause;
    pre_clause->nextClause = insert_clause;
    insert_clause->nextClause = p;
    formular.numClause ++;
}

/*
 * 判断子句是否为单子句
 */
inline bool isUnitClause(Clause* clause){
    return clause->firstLiteral != NULL && clause->firstLiteral->next == NULL;
}

void evaluateClause(){

}

/*
 * 判断是否存在单子句
 */
Clause* UnitClauseLeft(Formular formular){
    Clause* p = formular.root;
    while(p){
        if(isUnitClause(p))
            return p;
        p = p->nextClause;
    }
    return NULL;
}

/*
 * 复制公式
 */
Formular copyFormular(Formular formular){
    Formular newCopy;
    newCopy.numBoolen = formular.numBoolen;
    newCopy.numClause = formular.numClause;
    newCopy.root = NULL;

    if(formular.root){
        Clause* cls_old = formular.root;
        Clause* cls_new_head = (Clause*) malloc(sizeof(Clause));
//        if(cls_new_head->firstLiteral == NULL){
//            printf("malloc error!\n");
//        }
//        assert(cls_new_head != NULL);
        cls_new_head->firstLiteral = NULL;
        cls_new_head->nextClause = NULL;
        pLiteral lit_old = cls_old->firstLiteral;
        while(lit_old){
            Literal* lit_new_head = (Literal*) malloc(sizeof(Literal));
            lit_new_head->data = lit_old->data;
            lit_new_head->is_negative = lit_old->is_negative;
            lit_new_head->next = cls_new_head->firstLiteral;
            cls_new_head->firstLiteral = lit_new_head;
            lit_old = lit_old->next;
        }
        cls_old = cls_old->nextClause;
        while(cls_old){
            Clause* clause = (Clause*) malloc(sizeof(Clause));
            clause->firstLiteral = NULL;
            clause->nextClause = cls_new_head;
            cls_new_head = clause;
            newCopy.root = cls_new_head;
            lit_old = cls_old->firstLiteral;
            while(lit_old){
                Literal* lit_new_head = (Literal*) malloc(sizeof(Literal));
                lit_new_head->data = lit_old->data;
                lit_new_head->is_negative = lit_old->is_negative;
                lit_new_head->next = cls_new_head->firstLiteral;
                cls_new_head->firstLiteral = lit_new_head;
                lit_old = lit_old->next;
            }
            cls_old = cls_old->nextClause;
        }
    }
    return newCopy;
}

/*
 * DPLL算法
 */
void DPLL(Formular &formular, Answer& ans){
    Clause* unitClause;
    while(unitClause = UnitClauseLeft(formular)){
        int data = unitClause->firstLiteral->data;
        bool is_negative = unitClause->firstLiteral->is_negative;
        ans.state[data] = is_negative ? NEGATIVE : POSITIVE;
        Clause *s = formular.root;

        //依据单子句规则简化公式
        while(s){
            //如果子句为空则公式不可满足
            if(!s->firstLiteral){
                ans.solved = false;
                return;
            }

            //如果子句中存在单子句则删除该子句
            //当子句第一个变元为单子句时
            if(s->firstLiteral->data == data && s->firstLiteral->is_negative == is_negative){
                Clause* p = s;
                s = s->nextClause;
                destroyClause(formular, p);
            }
            //当子句第一个变元为相反的单子句时
            else if(s->firstLiteral->data == data && s->firstLiteral->is_negative != is_negative) {
                while(s->firstLiteral && s->firstLiteral->data == data && s->firstLiteral->is_negative != is_negative){
                    pLiteral p = s->firstLiteral;
                    s->firstLiteral = p->next;
                    free(p);
                }
                if(s->firstLiteral == NULL){
                    ans.solved = false;
                    return;
                }
                bool next = false;
                if(s->firstLiteral == NULL){
                    ans.solved = false;
                    return;
                } else{
                    pLiteral p = s->firstLiteral;
                    while(p->next){
                        if(p->next->data == data && p->next->is_negative == is_negative){
                            Clause* delClause = s;
                            s = s->nextClause;
                            next = true;
                            destroyClause(formular, delClause);
                            delClause = NULL;
                            p = NULL;
                            break;
                        }
                        else if(p->next->data == data && p->next->is_negative != is_negative){
                            pLiteral delLiteral = p->next;
                            p->next = delLiteral->next;
                            free(delLiteral);
                        }
                        else
                            p = p->next;
                    }
                    if(!next)
                        s = s->nextClause;
                }
            }
            else{
                pLiteral p = s->firstLiteral;
                bool next = false;
                while(p->next){
                    if(p->next->data == data && p->next->is_negative == is_negative){
                        Clause* delClause = s;
                        s = s->nextClause;
                        next = true;
                        destroyClause(formular, delClause);
                        delClause = NULL;
                        p = NULL;
                        break;
                    }
                    else if(p->next->data == data && p->next->is_negative != is_negative){
                        pLiteral delLiteral = p->next;
                        p->next = delLiteral->next;
                        free(delLiteral);
                    }
                    else
                        p = p->next;
                }
                if(!next)
                    s = s->nextClause;
            }
            if(formular.root == NULL){
                ans.solved = true;
                return;
            }
        }
        formular.numBoolen --;
    }

    //选取变元v
    pLiteral v = (pLiteral) malloc(sizeof(Literal));
    v->data = formular.root->firstLiteral->data;
    v->is_negative = false;
    v->next = NULL;
    Clause* clause = createClause(v);
    addClause(formular, formular.root, clause);
    Formular newFormular = copyFormular(formular);
    DPLL(newFormular, ans);
    if(ans.solved)
        return;
    else{
        destroyClause(formular, clause);
        v = (pLiteral) malloc(sizeof(Literal));
        v->data = formular.root->firstLiteral->data;
        v->is_negative = true;
        v->next = NULL;
        clause = createClause(v);
        addClause(formular, formular.root, clause);
        destroyFormular(newFormular);
        newFormular = copyFormular(formular);
        DPLL(newFormular, ans);
        if(ans.solved)
            return;
        else{
            ans.solved = false;
            return;
        }
    }
}

Answer DPLLSolution(Formular& formular){
    Answer ans;
    ans.numBoolen = formular.numBoolen;
    ans.state = (int*) malloc(sizeof(int) * (formular.numBoolen + 1));
    for(int i = 0; i <= formular.numBoolen; i ++)
        ans.state[i] = UNKNOWN;
    ans.solved = false;
    DPLL(formular, ans);
    return ans;
}

void destroyFormular(Formular& formular){
    Clause* p = formular.root;
    while(p){
        Clause* q = p;
        p = p->nextClause;
        destroyClause(formular, q);
    }
    formular.numBoolen = 0;
}

void destroyAnswer(Answer& ans){
    free(ans.state);
    ans.numBoolen = 0;
    ans.solved = false;
}

void test1(){
    printf("----SAT----\n");
}